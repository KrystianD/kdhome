import zmq, time, types, timerfd, os, sys

class KDHome:
	sub = None
	req = None
	sessKey = 0
	intervals = None
	timer = None

	def __init__(self):
		self.context = zmq.Context()
		self.intervals = []
		self.timer = timerfd.create(timerfd.CLOCK_REALTIME, 0)

	def connect(self, host, port):
		self.sub = self.context.socket(zmq.SUB)
		self.sub.connect("tcp://"+host+":9999")
		self.sub.setsockopt(zmq.SUBSCRIBE, b"#")

		self.req = self.context.socket(zmq.REQ)
		self.req.connect("tcp://"+host+":10000")

	def poll(self, time = -1, socks = []):
		poller = zmq.Poller()
		poller.register(self.sub, zmq.POLLIN)
		poller.register(self.timer, zmq.POLLIN)
		for v in socks:
			poller.register(v, zmq.POLLIN)
		d = dict(poller.poll(time))
		if self.timer in d:
			os.read(self.timer, 1024)
			print("TIMER")
		return d

	def process(self):

		for v in self.intervals:
			# print(v,time.time())
			if v["execTime"] <= time.time():
				if v["repeating"]:
					v["execTime"] = time.time() + v["interval"]

				code = v["code"]
				if callable(code):
					code()
				elif isinstance(code, str):
					eval(v["code"])

				if not v["repeating"]:
					print(v,"ER")
					self.intervals.remove(v)

				self.setupTimer()

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
			elif type == "MESSAGE":
				if cmd == "BROADCAST":
					msg = parts[2]
					self.onMessageEvent(msg)
			elif type == "INPUT":
				if cmd == "CHANGED":
					id = parts[2]
					name = parts[3]
					value = int(parts[4])
					self.onInputChangedEvent(id, name, value)
			elif type == "IR":
				if cmd == "NEW-CODE":
					code = int(parts[2], 16)
					self.onIRNewCodeEvent(code)
				elif cmd == "PRESSED":
					code = int(parts[2], 16)
					self.onIRPressedEvent(code)
				elif cmd == "RELEASED":
					code = int(parts[2], 16)
					self.onIRReleasedEvent(code)

			return True
		except zmq.error.Again:
			return False

	def request(self, msg):
		self.req.send((msg + ":" + str(self.sessKey)).encode("latin"))
		return self.req.recv()

	def getEvent(self):
		return None

	def broadcast(self, msg):
		resp = self.request("MESSAGE:BROADCAST:{0}".format(msg))
		return resp
	def log(self, msg):
		resp = self.request("MESSAGE:LOG:{0}".format(msg))
		return resp

	def registerEthernetDevice(self, id, ip, port, name):
		resp = self.request("CTRL:REGISTER-ETHERNET-DEVICE:{0}:{1}:{2}:{3}".format(id, ip, port, name))
		return int(resp)
	# def addOutputProvider(self, id, cnt):
		# self.request("CTRL:ADD-OUTPUT-PROVIDER:{0}:{1}".format(id, cnt))
	# def addInputProvider(self, id, cnt):
		# self.request("CTRL:ADD-INPUT-PROVIDER:{0}:{1}".format(id, cnt))
	# def addIRProvider(self, id):
		# self.request("CTRL:ADD-IR-PROVIDER:{0}".format(id))
	# def addTempProvider(self, id, cnt):
		# self.request("CTRL:ADD-TEMP-PROVIDER:{0}:{1}".format(id, cnt))

	def setOutput(self, name, value):
		self.request("OUTPUT:SET:{0}:{1}".format(name, int(value)))
	def toggleOutput(self, name):
		self.request("OUTPUT:TOGGLE:{0}".format(name))
	def getOutput(self, name):
		resp = self.request("OUTPUT:GET:{0}".format(name))
		return int(resp)
	def setOutputName(self, id, name):
		self.request("OUTPUT:SET-NAME:{}:{}".format(id, name))
	def setOutputPersistent(self, name):
		self.request("OUTPUT:SET-PERSISTENT:{}".format(name))

	def getInput(self, num):
		resp = self.request("INPUT:GET:{0}".format(num))
		return int(resp)
	def setInputName(self, id, name):
		self.request("INPUT:SET-NAME:{}:{}".format(id, name))

	def getTemp(self, num):
		resp = self.request("TEMP:GET:{0}".format(num))
		return float(resp)
	def isTempValid(self, num):
		resp = self.request("TEMP:IS-VALID:{0}".format(num))
		return int(resp)
	def setTempName(self, id, name):
		self.request("TEMP:SET-NAME:{}:{}".format(id, name))

	def setCounterName(self, id, name):
		self.request("COUNTER:SET-NAME:{}:{}".format(id, name))

	def onInputChangedEvent(self, id, name, value):
		pass
	def onIRNewCodeEvent(self, code):
		pass
	def onIRPressedEvent(self, code):
		pass
	def onIRReleasedEvent(self, code):
		pass

	def reset(self):
		self.request("CTRL:RESET")

	def onReset(self):
		pass
	def onInitEvent(self, ev):
		return False
	def onMessageEvent(self, msg):
		pass

	def run(self):
		while True:
			self.poll()
			self.process()
			sys.stdout.flush()
			sys.stderr.flush()

	def setTimeout(self, id, interval, code):
		self.setInterval(id, interval, code, False)

	def setInterval(self, id, interval, code, repeating = True):
		nextTime = time.time() + interval
		for v in self.intervals:
			if v["id"] == id:
				v["execTime"] = nextTime;
				v["repeating"] = repeating
				v["interval"] = interval
				v["code"] = code
				print("update interval")
				return
		v = { "id": id,
				"execTime": nextTime,
				"repeating": repeating,
				"interval": interval,
				"code": code }
		print("new interval")
		self.intervals.append(v)
		self.setupTimer()

	def hasTimeout(self, id):
		return self.hasInterval(id)

	def hasInterval(self, id):
		for v in self.intervals:
			if v["id"] == id:
				return True
		return False

	def removeTimeout(self, id):
		self.removeInterval(id)

	def removeInterval(self, id):
		for v in self.intervals:
			if v["id"] == id:
				self.intervals.remove(v)
				return
	
	def setupTimer(self):
		if len(self.intervals) > 0:
			earliest = min(self.intervals, key = lambda x: x["execTime"])
			delay = earliest["execTime"] - time.time()
			print("setting delay " + str(delay) + " for interval " + earliest["id"])
			timerfd.settime(self.timer, 0, delay, 0)

class InitEvent:
	pass
