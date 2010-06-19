 /*********************************************************
 *
 * This source code is part of the Carnegie Mellon Robot
 * Navigation Toolkit (CARMEN)
 *
 * CARMEN Copyright (c) 2002 Michael Montemerlo, Nicholas
 * Roy, Sebastian Thrun, Dirk Haehnel, Cyrill Stachniss,
 * and Jared Glover
 *
 * CARMEN is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option)
 * any later version.
 *
 * CARMEN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General
 * Public License along with CARMEN; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307 USA
 *
 ********************************************************/

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include "commander.h"

#define HEADER_LINE 3
#define START_LINE 7
#define LINE_WIDTH 1
#define ID_COLUMN 5
#define MODULE_COLUMN 9
#define HOST_COLUMN 36
#define STATE_COLUMN 50

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

int glob_nb_rows;
int glob_nb_cols;
char *glob_host_name;
WINDOW *glob_logwin = 0;
char *glob_config_filename = 0;

int nb_processes = 0;

carmen_commander_process *glob_processes = 0;
int glob_curr_proc_idx;
int logfile_idx = 0;

int carmen_commander_inside_logwin(int y, int x)
{
  if (glob_logwin == 0)
    return 0;

  if(y >= glob_logwin->_begy &&
     x >= glob_logwin->_begx &&
     y <= glob_logwin->_begy + glob_logwin->_maxy  &&
     x <= glob_logwin->_begx + glob_logwin->_maxx)
    return 1;
  return 0;
}

int carmen_commander_logwin_addch(char ch, int x, int y)
{
  static int xlogwin = 2;
  static int ylogwin = 1;

  if(glob_logwin == 0)
    return 0;

  if(x >= 0)
    xlogwin = x;
  if(y >= 0)
    ylogwin = y;

  if(xlogwin == glob_logwin->_maxx - 2 || ch == '\n'){
    ylogwin++;
    xlogwin = 2;
  }

  if(ylogwin == glob_logwin->_maxy - 1)
    return 0;

  if(ch == '\r')
    xlogwin = 2;

  if(ch != '\r' && ch != '\n')
    mvwaddch(glob_logwin, ylogwin, xlogwin, ch);

  xlogwin++;
  return 1;
}

void carmen_commander_set_hostname()
{
  FILE *bin_host;

  if (getenv("HOST") == NULL) {
    if (getenv("HOSTNAME") != NULL)
      setenv("HOST", getenv("HOSTNAME"), 1);
    else if (getenv("host") != NULL)
      setenv("HOST", getenv("host"), 1);
    else if (getenv("hostname") != NULL)
      setenv("HOST", getenv("hostname"), 1);
    else {
      bin_host = popen("/bin/hostname", "r");
      if (bin_host == NULL)
        carmen_commander_die(
          "Can't get machine name from $HOST, $host,"
		      "$hostname or /bin/hostname. Please set one"
		      "of these environment variables properly.", 1);
    }
  }

  glob_host_name = getenv("HOST");
}

int carmen_commander_accepts_pkauth(char *host)
{
  char cmd[450 + strlen(host)];
  int retval;

  sprintf(cmd,
  	  "/bin/sh -c \" ssh -o \"KbdInteractiveDevices=none\" "
  	  "-o \"HostbasedAuthentication=no\" "
  	  "-o \"PasswordAuthentication=no\"  "
  	  "-o \"PubkeyAuthentication=yes\" %s 'exit 0' &> /dev/null \" ",
	  host);
  retval = system(cmd);
  sprintf(cmd, "%d", retval);
  return (retval == 0);
}

int carmen_commander_test_host(char *host)
{
  int retval;
  char cmd[200];
  sprintf(cmd, "/bin/sh -c \" ping %s -q -w 1 -c 1 &> /dev/null\" ", host);
  retval = system(cmd);

  return(retval == 0);
}

void carmen_commander_print_state(int proc_idx)
{
  char state[] = " \0";
  int y = START_LINE + LINE_WIDTH * proc_idx;
  int x = STATE_COLUMN + 2;

  if(carmen_commander_inside_logwin(y, x))
    return;

  if(glob_processes[proc_idx].state == 1)
    state[0] = 'R';
  else if(glob_processes[proc_idx].state == 0)
    state[0] = 'N';
  else if(glob_processes[proc_idx].state == -1)
    state[0] = 'S';
  else if(glob_processes[proc_idx].state == -2)
    state[0] = 'H';
  else if(glob_processes[proc_idx].state == -3)
    state[0] = 'X';

  if(proc_idx == glob_curr_proc_idx)
    attron(A_STANDOUT);

  if(glob_processes[proc_idx].state == 1)
    attron(COLOR_PAIR(4));
  else
    attron(COLOR_PAIR(3));

  mvprintw(y, x, state);
  attroff(A_STANDOUT);
  attron(COLOR_PAIR(1));
  refresh();
}

void carmen_commander_print_proc_line(int idx)
{
  int len = strlen(glob_processes[idx].name);
  int i, x, y;

  getyx(stdscr, y, x);

  if(glob_processes[idx].x_forw)
   attron(COLOR_PAIR(2));
  else
    attron(COLOR_PAIR(1));

  if(idx >= 10)
    mvprintw(START_LINE + LINE_WIDTH * idx, ID_COLUMN -1, "%d", idx);
  else
    mvprintw(START_LINE + LINE_WIDTH * idx, ID_COLUMN, "%d", idx);

  for(i=0; i<MODULE_COLUMN - ID_COLUMN - 1; i++)
    addch(' ');
  printw("%s", glob_processes[idx].name);

  for(i=0; i<HOST_COLUMN - MODULE_COLUMN - len; i++)
    addch(' ');

  printw("%s", glob_processes[idx].host_name);
  len = strlen(glob_processes[idx].host_name);
  for(i=0; i<STATE_COLUMN - HOST_COLUMN - len + 2; i++)
    addch(' ');

  carmen_commander_print_state(idx);
  move(y, x);
  refresh();
}

void carmen_commander_highlight(int idx)
{
  attron(A_STANDOUT);
  carmen_commander_print_proc_line(idx);
  attroff(A_STANDOUT);
}

void carmen_commander_unhighlight(int idx)
{
  attroff(A_STANDOUT);
  carmen_commander_print_proc_line(idx);
}

void carmen_commander_start_process(carmen_commander_process *proc)
{
  char *arg[4], remote_cmd[500], host_name[256];
  char msg[100], *central_host = 0;
  int child_pid, remote = 0, i;

  /* look for the central process */
  for(i=0; i<nb_processes; i++) {
    unsigned int name_len = strlen(glob_processes[i].name);
    if ((name_len >= 7) &&
      !strcmp(&glob_processes[i].name[name_len-7], "central"))
      central_host = strdup(glob_processes[i].host_name);
  }

  if(central_host == 0)
    /* find central host */
    central_host = getenv("CENTRALHOST");

  if(central_host == 0) /* no central process found, use this host */
    central_host = strdup(glob_host_name);

  setenv("CENTRALHOST", central_host, 1);

  /* setup arguments */
  arg[0] = (char *)malloc((strlen("sh") + 1) * sizeof(char));
  strcpy(arg[0], "sh");

  arg[1] = (char *)malloc((strlen("-c") + 1) * sizeof(char));
  strcpy(arg[1], "-c");

  gethostname(host_name, sizeof(host_name));
  if (!strcmp(proc->host_name, "localhost") ||
    !strcmp(proc->host_name, host_name)) {
    arg[2] = (char *)malloc((strlen(proc->call) + 1) * sizeof(char));
    strcpy(arg[2], proc->call);
  }

  else{ // remote host

    if(proc->x_forw)
      sprintf(remote_cmd, "ssh -X %s 'export CENTRALHOST=%s; %s'",
	      proc->host_name, central_host, proc->call);
    else
      sprintf(remote_cmd, "ssh %s 'export CENTRALHOST=%s; %s'",
	      proc->host_name, central_host, proc->call);
    arg[2] =  (char *)malloc((strlen(remote_cmd) + 1) * sizeof(char));
    strcpy(arg[2], remote_cmd);
    remote = 1;
    free(central_host);
  }

  arg[3] = NULL;

  proc->log_file_name = (char *)malloc((strlen(proc->name) + 100) *
				       sizeof(char));
  proc->infile_name = (char *)malloc((strlen(proc->name) + 100) *
				       sizeof(char));

  sprintf(proc->log_file_name, "/tmp/%s_%02d.commander", proc->name, logfile_idx);
  sprintf(proc->infile_name, "/tmp/%s_%02d_in.commander", proc->name, logfile_idx);

  if (remote){

    /* check whether remote host is alive */
    if(!carmen_commander_test_host(proc->host_name)){
      carmen_commander_print_message("Host not found! (Could not ping)");
      return;
    }

    /* check whether remote host accepts
     * public key authentication */
    if(!carmen_commander_accepts_pkauth(proc->host_name)){
      carmen_commander_print_message("Public key authentication denied");
      return;
    }
  }

  if((child_pid = fork()) == 0){

    FILE *fp_out = fopen(proc->log_file_name, "w");
    FILE *fp_in  = fopen(proc->infile_name, "r");
    if(!fp_in){
      fp_in = fopen(proc->infile_name, "w");
      fclose(fp_in);
      fp_in = fopen(proc->infile_name, "r");
    }

    fseek(fp_in, 0, SEEK_END);

    dup2(fileno(fp_in), fileno(stdin));
    dup2(fileno(fp_out), fileno(stdout));
    dup2(fileno(fp_out), fileno(stderr));

    execv("/bin/sh", arg);

    fclose(fp_in);
    fclose(fp_out);
    exit(0);
  }

  logfile_idx++;
  proc->pid = child_pid;

  sprintf(msg, "Started %s", proc->name);
  carmen_commander_print_message(msg);
}

void carmen_commander_kill_process(carmen_commander_process *proc)
{
  char msg[100], host_name[256];
  char kill_cmd[200];
  char cmd_line[strlen(proc->call)];
  char args[strlen(proc->call)];

  gethostname(host_name, sizeof(host_name));
  if (strcmp(proc->host_name, "localhost") &&
    strcmp(proc->host_name, host_name)) {
    /* remote host */

    sscanf(proc->call, "%s %[^\n]", cmd_line, args);

    if(proc->is_genom)
      sprintf(kill_cmd, "ssh %s 'killmodule %s >& /dev/null'", proc->host_name, proc->name);
    else
      sprintf(kill_cmd, "ssh %s 'killall -q %s'", proc->host_name, cmd_line);
    system(kill_cmd);
  }

  if(proc->pid > 1){
    if(proc->is_genom){
      sprintf(kill_cmd, "killmodule %s >& /dev/null", proc->name);
      system(kill_cmd);
    }
    else{
      kill(proc->pid, SIGINT);
      //kill(proc->pid, SIGTERM);
      //      kill(proc->pid, SIGKILL);
    }
  }

  sprintf(msg, "Killed %s", proc->name);
  carmen_commander_print_message(msg);
}


void carmen_commander_print_centered(char const *str, int row)
{
  mvprintw(row, (glob_nb_cols - strlen(str))/2, str);
  refresh();
}

void carmen_commander_print_header(int header_row)
{
  carmen_commander_print_centered("CARMEN PROCESS COMMANDER", header_row);

  mvprintw(header_row + 2, ID_COLUMN - 1, "ID");
  mvprintw(header_row + 2, MODULE_COLUMN, "MODULE NAME");
  mvprintw(header_row + 2, HOST_COLUMN, "HOST");
  mvprintw(header_row + 2, STATE_COLUMN, "STATE");

  refresh();
}

void carmen_commander_print_processes()
{
  int i=0;
  for(i=0; i<nb_processes; i++){
    if(i == glob_curr_proc_idx)
      carmen_commander_highlight(i);
    else
      carmen_commander_unhighlight(i);
  }
}

void carmen_commander_print_footer()
{
  int footer_row = glob_nb_rows - 9;
  mvprintw(footer_row, 5, "Commands:");
  mvaddch(footer_row + 1, 5, 'R' | A_UNDERLINE);
  printw("un process    ");

  addch('K' | A_UNDERLINE);
  printw("ill process    ");

  addch('D' | A_UNDERLINE);
  printw("isplay process output    ");

  addch('Q' | A_UNDERLINE);
  printw("uit commander    ");

  mvaddch(footer_row + 2, 5, 'U' | A_UNDERLINE);
  printw("pdate    ");

  mvprintw(footer_row + 4, 5, "Process states:");
  move(footer_row + 5, 5);

  addch('R' | A_UNDERLINE);
  printw("unning    ");

  addch('S' | A_UNDERLINE);
  printw("ignalled    ");

  addch('H' | A_UNDERLINE);
  printw("alted    ");

  addch('e');
  addch('X' | A_UNDERLINE);
  printw("ited    ");

  addch('N' | A_UNDERLINE);
  printw("ot started    ");

  refresh();
}

void carmen_commander_read_config_file()
{
  char proc_name[200], host_name[200], proc_call[200];
  char first_char, line_char, last_char;
  FILE *fp = fopen(glob_config_filename, "r");

  if(fp == NULL){
    char msg[255];
    sprintf(msg, "Could not open config file '%s'",
	    glob_config_filename);
    carmen_commander_die(msg, 1);
  }

  while(1){

    if(fscanf(fp, " %c", &first_char) != 1)
      break;

    if(first_char == '#'){
      do fread(&line_char, 1, 1, fp);
      while(line_char != '\n');
      continue;
    }

    if(fscanf(fp, " %s %s %[^\n]", &proc_name[1], host_name, proc_call) != 3)
      break;
    proc_name[0] = first_char;

    if(fscanf(fp, "%c", &last_char) != 1)
      break;

    glob_processes = (carmen_commander_process*) realloc(glob_processes,
					      (nb_processes+1) *
					      sizeof(carmen_commander_process));
    glob_processes[nb_processes].call = strdup(proc_call);
    glob_processes[nb_processes].state = 0;
    glob_processes[nb_processes].pid = -1;
    glob_processes[nb_processes].log_file_name = 0;


    if(strlen(proc_name) > 2 &&
       proc_name[0] == 'g' && proc_name[1] == ':'){
      glob_processes[nb_processes].is_genom = 1;
      glob_processes[nb_processes].name = strdup(&proc_name[2]);
    }
    else {
      glob_processes[nb_processes].is_genom = 0;
      glob_processes[nb_processes].name = strdup(proc_name);
    }

    if(strlen(host_name) > 2 &&
       host_name[0] == 'x' && host_name[1] == ':'){
      glob_processes[nb_processes].x_forw = 1;
      glob_processes[nb_processes].host_name = strdup(&host_name[2]);
    }
    else{
      glob_processes[nb_processes].x_forw = 0;
      glob_processes[nb_processes].host_name = strdup(host_name);
    }

    nb_processes++;
  }
  fclose(fp);
}

void carmen_commander_die(char *msg, int exit_code)
{
  char *nmsg = (char*) malloc(strlen(msg) * sizeof(char) + 25);
  sprintf(nmsg, "%s Press any key to quit.", msg);
  carmen_commander_print_message(nmsg);
  free(nmsg);
  nocbreak();
  raw();
  getch();
  endwin();
  exit(exit_code);
}

void carmen_commander_clear_processes()
{
  int i=0;
  for(i=0; i<nb_processes; i++){

    carmen_commander_kill_process(&glob_processes[i]);

    if(glob_processes[i].name)
      free(glob_processes[i].name);
    if(glob_processes[i].call)
      free(glob_processes[i].call);
    if(glob_processes[i].host_name)
      free(glob_processes[i].host_name);
  }
  free(glob_processes);
  glob_processes = NULL;
  nb_processes = 0;
}

int carmen_commander_init(int argc, char **argv)
{
  initscr();
  raw();
  noecho();
  keypad(stdscr, TRUE);
  halfdelay(1);
  curs_set(0);
  getmaxyx(stdscr, glob_nb_rows, glob_nb_cols);
  start_color();
  use_default_colors();
  init_pair(1, -1, -1);
  init_pair(2, COLOR_CYAN, -1);
  init_pair(3, COLOR_RED, -1);
  init_pair(4, COLOR_GREEN, -1);
  init_pair(5, COLOR_BLUE, -1);

  carmen_commander_set_hostname();

  return 0;
}

void carmen_commander_exit()
{
  endwin();
  exit(1);
}

int carmen_commander_quit()
{
  char answ = ' ';
  carmen_commander_print_message("Kill all processes? (y/n/c)");

  while(answ != 'y' && answ != 'n' &&
	answ != 'c' && answ != 'Y' &&
	answ != 'N' && answ != 'C'){
    answ = getchar();
  }

  if(answ == 'c' || answ == 'C')
    return 0;

  if(answ == 'y' || answ == 'Y')
    carmen_commander_clear_processes();

  return 1;
}

void carmen_commander_print_message(char const *msg)
{
  int x, y;
  getyx(stdscr, y, x);
  attron(A_BOLD);
  attron(COLOR_PAIR(5));
  mvprintw(glob_nb_rows - 2, 5, "%s\t\t\t", msg);
  move(y, x);
  attroff(A_BOLD);
  attroff(COLOR_PAIR(5));
  refresh();
}

void carmen_commander_make_menu(char *config_file_name)
{
  if(config_file_name != NULL)
    glob_config_filename = strdup(config_file_name);

  carmen_commander_read_config_file();

  carmen_commander_print_header(HEADER_LINE);

  glob_curr_proc_idx = 0;

  carmen_commander_print_processes();

  carmen_commander_print_footer();
}


void carmen_commander_arrow_up()
{
  carmen_commander_unhighlight(glob_curr_proc_idx);

  if(glob_curr_proc_idx == 0)
    glob_curr_proc_idx = nb_processes -1;
  else
    glob_curr_proc_idx--;

  carmen_commander_highlight(glob_curr_proc_idx);
}

void carmen_commander_arrow_down()
{
  carmen_commander_unhighlight(glob_curr_proc_idx);

  if(glob_curr_proc_idx == nb_processes-1){
    glob_curr_proc_idx =  0;
  }
  else{
    glob_curr_proc_idx++;
  }

  carmen_commander_highlight(glob_curr_proc_idx);
}

void carmen_commander_run_process()
{
  if(glob_processes[glob_curr_proc_idx].state != 1)
    carmen_commander_start_process(&glob_processes[glob_curr_proc_idx]);
}

void carmen_commander_stop_process()
{
  carmen_commander_kill_process(&glob_processes[glob_curr_proc_idx]);
}

void carmen_commander_show_process()
{
  int height, width, starty, startx;
  int nb_chars;
  char in_ch = ' ', log_ch = ' ';
  FILE *fp_out, *fp_in;

  height = glob_nb_rows - 15;
  width  = MIN(70, glob_nb_cols - 10);
  starty = 5;
  startx = glob_nb_cols - width - 5;

  if(glob_logwin == 0){

    fp_in  = fopen(glob_processes[glob_curr_proc_idx].infile_name, "a");
    fp_out = fopen(glob_processes[glob_curr_proc_idx].log_file_name, "r");

    if(!fp_out){
      carmen_commander_print_message("No output logfile found!");
      return;
    }

    glob_logwin = newwin(height, width, starty, startx);
    box(glob_logwin, 0, 0);
    mvwprintw(glob_logwin, 0, 3, "Output of %s, <d> to dismiss",
	      glob_processes[glob_curr_proc_idx].name);

    fseek(fp_out, -(height-2) * (width-2), SEEK_END);
    carmen_commander_logwin_addch(' ', 1, 1);

    while(in_ch != 'd'){
      in_ch = wgetch(glob_logwin);

      if(in_ch == 'r' || in_ch == 'R')
	carmen_commander_run_process();
      else if (in_ch == 'k' || in_ch == 'K')
	carmen_commander_stop_process();
      else if (in_ch > 0){
      	fprintf(fp_in, "%c", in_ch);
      	fflush(fp_in);
      }

      nb_chars = 0;
      while(read(fileno(fp_out), &log_ch, 1) && nb_chars < 1000){

	if(!carmen_commander_logwin_addch(log_ch, -1, -1)){
	  wclear(glob_logwin);
	  box(glob_logwin, 0, 0);
	  mvwprintw(glob_logwin, 0, 3, "Output of %s, <q> to quit",
		    glob_processes[glob_curr_proc_idx].name);
	  carmen_commander_logwin_addch(log_ch, 2, 2);
	}

      	wrefresh(glob_logwin);
	nb_chars++;
      }
      carmen_commander_watch_processes();
    }
    wrefresh(glob_logwin);

    fclose(fp_out);
  }
  clear();
  wborder(glob_logwin, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  wrefresh(glob_logwin);
  delwin(glob_logwin);
  glob_logwin = 0;
  carmen_commander_print_header(HEADER_LINE);
  carmen_commander_print_processes();
  carmen_commander_print_footer();
  refresh();
}

void carmen_commander_update()
{
  carmen_commander_clear_processes();
  clear();
  carmen_commander_make_menu(NULL);
}


void carmen_commander_watch_processes()
{
  int i, err, state;

  for(i = 0; i < nb_processes; i++){


    if(glob_processes[i].pid >= 0){
      err = waitpid(glob_processes[i].pid, &state, WNOHANG);

      if(err != 0){
	glob_processes[i].state = 0;
	if(WIFSIGNALED(state)) {
	  glob_processes[i].state = -1;
	}
	if(WIFSTOPPED(state)) {
	  glob_processes[i].state = -2;
	}
	if(WIFEXITED(state)) {
	  glob_processes[i].state = -3;
	}
      }
      else
	glob_processes[i].state = 1;
    }
    else
      glob_processes[i].state = 0;

    carmen_commander_print_state(i);
  }
}
