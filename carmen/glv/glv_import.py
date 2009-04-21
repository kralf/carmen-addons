#!BPY

"""
Name: 'GLV file (.glv)...'
Blender: 241
Group: 'Import'
Tooltip: 'Import from GLV file format (.glv)'
"""

############################################################################
#    Copyright (C) 2007 by Ralf 'Decan' Kaestner                           #
#    ralf.kaestner@gmail.com                                               #
#                                                                          #
#    This program is free software; you can redistribute it and#or modify  #
#    it under the terms of the GNU General Public License as published by  #
#    the Free Software Foundation; either version 2 of the License, or     #
#    (at your option) any later version.                                   #
#                                                                          #
#    This program is distributed in the hope that it will be useful,       #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#    GNU General Public License for more details.                          #
#                                                                          #
#    You should have received a copy of the GNU General Public License     #
#    along with this program; if not, write to the                         #
#    Free Software Foundation, Inc.,                                       #
#    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
############################################################################

__author__ = "Ralf Kaestner"

import Blender
from Blender import Scene
from Blender.Scene import *
from Blender import Group
from Blender.Group import *
from Blender import Object
from Blender.Object import *
from Blender import Material
from Blender.Material import *
from Blender import NMesh
from Blender.NMesh import *

import re
from re import *
import array

""" Blender raw parts import script
    @author Ralf Kaestner SU Computer Science Dept.
"""


def exitCorrupted(message):
  print "|-- Corrupted file: %s" % (message)
  sys.exit(1)

def importGLV(filename):
  match = re.compile(".*/(\w*)\..*", DOTALL).match(filename)
  name = match.group(1)

  file = open(filename, "rb")
  scene = Scene.GetCurrent()

  mesh = NMesh.New(name)

  print "Importing mesh object "+name+" from "+filename

  while 1:
    try:
      identifier = array.array('B')
      identifier.fromfile(file, 1)
    except:
      break

    if (identifier[0] == 0):
      v = array.array('f')
      v.fromfile(file, 3)

      vertex = NMesh.Vert(v[0], v[1], v[2])
      mesh.verts.append(vertex)
    elif (identifier[0] == 1):
      v1 = array.array('f')
      v1.fromfile(file, 3)
      vertex1 = NMesh.Vert(v1[0], v1[1], v1[2])
      mesh.verts.append(vertex1)

      v2 = array.array('f')
      v2.fromfile(file, 3)
      vertex2 = NMesh.Vert(v2[0], v2[1], v2[2])
      mesh.verts.append(vertex2)

      mesh.addEdge(vertex1, vertex2)
    elif (identifier[0] == 2):
      v1 = array.array('f')
      v1.fromfile(file, 3)
      vertex1 = NMesh.Vert(v1[0], v1[1], v1[2])
      mesh.verts.append(vertex1)

      v2 = array.array('f')
      v2.fromfile(file, 3)
      vertex2 = NMesh.Vert(v2[0], v2[1], v2[2])
      mesh.verts.append(vertex2)

      v3 = array.array('f')
      v3.fromfile(file, 3)
      vertex3 = NMesh.Vert(v3[0], v3[1], v3[2])
      mesh.verts.append(vertex3)

      face = NMesh.Face()
      face.v.append(vertex1)
      face.v.append(vertex2)
      face.v.append(vertex3)

      mesh.faces.append(face)
    elif (identifier[0] == 3):
      color = array.array('B')
      color.fromfile(file, 3)
    else:
      exitCorrupted("Unknown identifier")

  object = NMesh.PutRaw(mesh)

  file.close()
  print "Success"

  Blender.Redraw();

if __name__ == "__main__":
  Blender.Window.FileSelector(importGLV, "Import GLV file", "*.glv")
