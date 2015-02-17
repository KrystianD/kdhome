#!/usr/bin/python
import struct, time, sys, select, os
from fcntl import fcntl, F_GETFL, F_SETFL

listener = None
kbdPath = None
mousePath = None
keys = {}

#long int, long int, unsigned short, unsigned short, unsigned int
FORMAT = 'llHHI'
EVENT_SIZE = struct.calcsize(FORMAT)

class IInputListener:
  def onKeyPressed(self, key):
    pass
  def onKeyReleased(self, key):
    pass
  def onMouseActivity(self):
    pass

def init():
  global kbdFile, mouseFile, lastEvent, isIdle
  kbdFile = None
  mouseFile = None
  lastEvent = time.time()
  isIdle = False

def getFds():
  global kbdFile, mouseFile
  v = []
  if kbdFile is not None:
    v.append(kbdFile)
  if mouseFile is not None:
    v.append(mouseFile)
  return v

def setKeyboardPath(path):
	global kbdPath
	kbdPath = path

def setMousePath(path):
	global mousePath
	mousePath = path

def check():
  global kbdFile, mouseFile, lastEvent, isIdle, listener

  v = []
  if kbdFile:
    v.append(kbdFile)
  if mouseFile:
    v.append(mouseFile)
  r = select.select(v, [], [], 0)

  if kbdFile in r[0]:
    try:
      event = kbdFile.read(1024)

      while len(event) >= EVENT_SIZE:
        singleEvent = event[0:EVENT_SIZE]
        event = event[EVENT_SIZE:]

        (tv_sec, tv_usec, type, code, value) = struct.unpack(FORMAT, singleEvent)

        if type == 1:
          # if value == 1 or value == 2: # pressed or repeated
          if value == 1:
            keys[code] = True
            if listener is not None:
              listener.onKeyPressed(code)
          elif value == 0: # released
            keys[code] = False
            if listener is not None:
              listener.onKeyReleased(code)

        lastEvent = time.time()

    except KeyboardInterrupt:
      raise
    except OSError:
      kbdFile = None

  if mouseFile in r[0]:
    try:
      event = mouseFile.read(EVENT_SIZE)
      if listener is not None:
        listener.onMouseActivity()
      lastEvent = time.time()
    except KeyboardInterrupt:
      raise
    except OSError:
      mouseFile = None

  if not kbdFile:
    try:
      kbdFile = open(kbdPath, "rb")

      flags = fcntl(kbdFile.fileno(), F_GETFL)
      fcntl(kbdFile.fileno(), F_SETFL, flags | os.O_NONBLOCK)
    except OSError:
      print("unable to open kbd")

  if not mouseFile:
    try:
      mouseFile = open(mousePath, "rb")
    except OSError:
      print("unable to open mouse")

def getIdleTime():
  global lastEvent
  return time.time() - lastEvent

def isKeyPressed(key):
  global keys
  if key not in keys:
    return False
  return keys[key]
