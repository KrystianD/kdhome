import time

class KDIdleTimer:
  lastEvent = None
  idleTime = None
  idleCallback = None
  busyCallback = None

  def __init__(self, idleTime):
    self.lastEvent = -1
    self.idleTime = idleTime

  def setIdleTime(self, idleTime):
    self.idleTime = idleTime;

  def wake(self):
    if self.lastEvent == -1 and self.busyCallback:
      self.busyCallback()
    self.lastEvent = time.time()

  def process(self):
    if self.lastEvent != -1 and time.time() - self.lastEvent >= self.idleTime:
      self.lastEvent = -1
      if self.idleCallback:
        self.idleCallback()
