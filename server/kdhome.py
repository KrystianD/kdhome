import zmq, time

class KDHome:
	sub = None
	req = None

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

			if message == "INIT":
				ev = InitEvent()
				if self.onInitEvent(ev):
					self.request("CTRL:INIT-OK")

			return True
		except zmq.error.Again:
			return False
	
	def request(self, msg):
		self.req.send(msg.encode("latin"))
		self.req.recv()

	def getEvent(self):
		return None

	def registerEthernetDevice(self, id, ip, port):
		self.request("CTRL:REGISTER-ETHERNET-DEVICE:{0}:{1}:{2}".format(id, ip, port))

	def onInitEvent(self, ev):
		return False

class InitEvent:
	pass
