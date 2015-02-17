import time

class KDState:
	name = None
	curState = None
	stateChangedCallback = None
	stateChangedTime = None
	eachStateChangedTime = None
	delayTime = None

	def __init__(self, name, initState):
		self.name = name
		self.curState = None
		self.delayTime = 0
		self.changeState(initState)

	def changeState(self, newState, delayNextChange = 0):
		if time.time() < self.delayTime:
			return
		if newState != self.curState:
			oldState = self.curState
			self.curState = newState
			if self.stateChangedCallback:
				self.stateChangedCallback(self, oldState, newState)
			self.stateChangedTime = time.time()
			self.delayTime = time.time() + delayNextChange
		self.eachStateChangedTime = time.time()

	def getElapsedTime(self):
		return time.time() - self.stateChangedTime

	def getState(self):
		return self.curState

	def isInState(self, state):
		return self.curState == state

	def __str__(self):
		return "[KDState {0}]".format(self.name)

class KDTimedState(KDState):
	idleState = None
	keepStates = None
	idleTimeout = None

	def __init__(self, name, idleState, idleTimeout = 0):
		super().__init__(name, idleState)

		self.idleState = idleState
		self.keepStates = [idleState]
		self.idleTimeout = idleTimeout

	def process(self):
		if self.idleTimeout != 0 and \
				self.getState() not in self.keepStates and \
				time.time() - self.eachStateChangedTime >= self.idleTimeout:

			self.changeState(self.idleState)

	def keepState(self, state):
		if state not in self.keepStates:
			self.keepStates.append(state)

	def __str__(self):
		return "[KDTimedState {0}]".format(self.name)
