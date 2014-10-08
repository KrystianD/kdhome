import zmq, time

class KDHome:
	sub = None
	req = None
	sessKey = 0

	def __init__(self):
		self.context = zmq.Context()

	def connect(self, host, port):
		self.sub = self.context.socket(zmq.SUB)
		self.sub.connect("tcp://127.0.0.1:9999")
		self.sub.setsockopt(zmq.SUBSCRIBE, b"#")

		self.req = self.context.socket(zmq.REQ)
		self.req.connect("tcp://127.0.0.1:10000")

	def process(self):
		try:
			message = self.sub.recv(zmq.NOBLOCK)
			print("New ZMQ message: " + message.decode("latin2"))
			message = message[1:].decode("latin")

			parts = message.split(":")
			sessKey = int(parts[-1])

			type = parts[0]
			if self.sessKey != sessKey:
				self.onReset()
				self.sessKey = sessKey
				print("New session")

			cmd = parts[1]
			if type == "CTRL":
				if cmd == "INIT":
					ev = InitEvent()
					if self.onInitEvent(ev):
						self.request("CTRL:INIT-OK")
			elif type == "INPUT":
				if cmd == "CHANGED":
					num = int(parts[2])
					value = int(parts[3])
					self.onInputChangedEvent(num, value)
			elif type == "IR":
				if cmd == "NEW-CODE":
					code = int(parts[2])
					self.onIRNewCodeEvent(code)
				elif cmd == "PRESSED":
					code = int(parts[2])
					self.onIRPressedEvent(code)
				elif cmd == "RELEASED":
					code = int(parts[2])
					self.onIRReleasedEvent(code)

			return True
		except zmq.error.Again:
			return False
	
	def request(self, msg):
		self.req.send((msg + ":" + str(self.sessKey)).encode("latin"))
		return self.req.recv()

	def getEvent(self):
		return None

	def registerEthernetDevice(self, id, ip, port):
		resp = self.request("CTRL:REGISTER-ETHERNET-DEVICE:{0}:{1}:{2}".format(id, ip, port))
		return int(resp)
	def addOutputProvider(self, id, cnt):
		self.request("CTRL:ADD-OUTPUT-PROVIDER:{0}:{1}".format(id, cnt))
	def addInputProvider(self, id, cnt):
		self.request("CTRL:ADD-INPUT-PROVIDER:{0}:{1}".format(id, cnt))
	def addIRProvider(self, id):
		self.request("CTRL:ADD-IR-PROVIDER:{0}".format(id))
	def addTempProvider(self, id, cnt):
		self.request("CTRL:ADD-TEMP-PROVIDER:{0}:{1}".format(id, cnt))

	def setOutput(self, num, value):
		self.request("OUTPUT:SET:{0}:{1}".format(num, value))
	def toggleOutput(self, num):
		self.request("OUTPUT:TOGGLE:{0}".format(num))
	def getOutput(self, num):
		resp = self.request("OUTPUT:GET:{0}".format(num))
		return int(resp)

	def getInput(self, num):
		resp = self.request("INPUT:GET:{0}".format(num))
		return int(resp)

	def getTemp(self, num):
		resp = self.request("TEMP:GET:{0}".format(num))
		return float(resp)
	def isTempValid(self, num):
		resp = self.request("TEMP:IS-VALID:{0}".format(num))
		return int(resp)

	def onReset(self):
		pass
	def onInitEvent(self, ev):
		return False

class InitEvent:
	pass
