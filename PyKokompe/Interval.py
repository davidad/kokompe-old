#coding=utf-8
#
#	Kokompe Interval Algebra module for Python
#
#	Smári McCarthy <smari@yaxic.org>
#	Based on interval.cpp by Ara Knaian <ara@mit.edu>
#
#	Copyright © FAB Foundation, 2008.
#	Copyright © Massachusetts Institute of Technology, 2007.
#


class Interval:
	"""
	Implements real interval algebra.
	
	We store two values:  a and b.
	Agnostic towards infinity. 
	Real numbers are considered to be intervals of length 0 and vice versa.
	"""
	
	def __init__(self, a=None, b=None):
		#
		#	Data invariant:   a < b
		#
		self.a = a
		self.b = b
		pass
	
	def SetRealInterval(self, a, b):
		"""
		We demand correct order on a, b here, but elsewhere we're agnostic.
		"""
		if (b < a):
			raise OutOfOrderException
		self.a = a
		self.b = b
		pass
	
	def SetRealNumber(self, a):
		"""
		Change an interval into a real number.
		"""
		self.a = a
		self.b = a
		return None
		
	def GetCenter(self):
		"""
		Return arithmetic average of the interval.
		"""
		return (self.a+self.b)*0.5
		
	def GetLength(self):
		"""
		Length of interval. Uses British Rail metric.
		"""
		return abs(self.b-self.a)
		
	def GetUpper(self):
		"""
		Returns upper value.
		"""
		return self.a
			
	def GetLower(self):
		"""
		Returns lower value.
		"""
		return self.a
	
	def Overlaps(self, other):
		"""
		Returns true if two Intervals overlap.
		"""
		if not isinstance(other, Interval):
			raise TypeError("Only Intervals can overlap with Intervals.")
		return ( ((other.a >= self.a) and (other.a <= self.b)) or ((self.a >= other.a) and (self.a <= other.b)) );
		
	def IsRealNumber(self):
		"""
		Returns true if the Interval represents a real number.
		"""
		if self.a == self.b:
			return True
		return False
		
	def GetRealValue(self):
		"""
		Returns real value iff the Interval represents a real number. Otherwise returns None.
		"""
		if not self.IsRealNumber():
			return None
		return self.a
	
	def __add__(self, other):
		"""
		Add two intervals to get their vector sum.
		Add a real number to an interval to lengthen it.
		"""
		c = Interval()
		if type(other) == int or type(other) == float:
			c.SetRealInterval(self.a, self.b+other)
		elif isinstance(other, Interval):
			c.SetRealInterval(self.a+other.a, self.b+self.b)
			if c.b < c.a:
				temp = c.a
				c.a = c.b
				c.b = temp
		else:
			raise TypeError("You can only add Intervals or real numbers from Intervals")
		return c
		
	def __sub__(self, other):
		"""
		Subtract two intervals to get their vector difference.
		Subtract a real number from an Interval to shorten it.
		"""
		c = Interval()
		if type(other) == int or type(other) == float:
			c.SetRealInterval(self.a, self.b-other)
		elif isinstance(other, Interval):
			c.SetRealInterval(self.a-other.b, self.b-other.a)
			if c.b < c.a:
				temp = c.a
				c.a = c.b
				c.b = temp
		else:
			raise TypeError("You can only subtract Intervals or real numbers from Intervals")
		return c
		
	def __mul__(self, other):
		"""
		Multiply an Interval with with:
			a) Another Interval.   
				For Intervals A, B, then 
					A·B = [a1, a2]·[b1, b2] 
					= [min(a1b1, a1b2, a2b1, a2b2), max(a1b1, a1b2, a2b1, a2b2)]
			b) A boolean value.
				For Interval A and bool b.  Ab = ( (b => A) ^ (¬b => 0) )
			c) A real number.
				For Interval A and real c, cA = c[a1, a2] = [ca1, ca2].
		"""
		i = Interval()
		if isinstance(other, Interval):
			opts = [self.a * other.a, self.a * other.b, self.b * other.a, self.b * other.b]
			i.SetRealInterval(min(opts), max(opts))
		elif type(other) == bool:
			if other == True:
				i.SetRealInterval(self.a, self.b)
			else:
				i.SetRealInterval(0, 0)
		elif type(other) == int or type(other) == float:
			i.SetRealInterval(self.a * other, self.b * other)
		else:
			raise TypeException("Right side of multiplication with Interval is defined for Interval, bool, int and float.")
		
		return i

	def __div__(self, other):
		"""
		Divide an Interval by:
			a) Another Interval.
				For Intervals A, B; 0 \notin B, then
					A/B = [a1, a2]·[b1, b2] 
					= [min(a1/b1, a1/b2, a2/b1, a2/b2), max(a1/b1, a1/b2, a2/b1, a2/b2)]
			b) A real number.
				For Interval A an real c, A/c = 1/c * [a1, a2]. See multiplication (__mul__) for resolution.
		"""
		i = Interval()
		if isinstance(other, Interval):
			if other.a == 0 or other.b == 0:
				raise ZeroDivisionError("Interval division by zero-Interval [a,0] or [a,0]")
			i = opts = [self.a / other.a, self.a / other.b, self.b / other.a, self.b / other.b]
			i.SetRealInterval(min(opts), max(opts))
		elif type(other) == int or type(other) == float:
			if other == 0:
				raise ZeroDivisionError("Interval division by zero")
			i.SetRealInterval(self.a / other, self.b / other)
		else:
			raise TypeException("Right side of division for Interval is defined for Interval, int and float.")
		
		return i
		
	def __pow__(self, other):
		# FIXME: Unimplemented 
		raise NotImplemented
	
	def __lshift__(self, other):
		"""
		Left shift operator shifts the Interval left by the value of 'other', without changing it's length.
		"""
		i = Interval()
		if type(other) == int or type(other) == float:
			i.SetRealInterval(self.a - other, self.b - other)
		else:
			raise TypeError("You can only shift Intervals by real numbers")
		return i
		
	def __rshift__(self, other):
		"""
		Right shift operator shifts the Interval right by the value of 'other', without changing it's length.
		"""
		i = Interval()
		if type(other) == int or type(other) == float:
			i.SetRealInterval(self.a + other, self.b + other)
		else:
			raise TypeError("You can only shift Intervals by real numbers")
		return i
		
	def __and__(self, other):
		"""
		a & b : Returns the largest contiguous interval shared by both intervals, or an
		empty interval if they are disjoint.
		"""
		if not isinstance(other, Interval):
			raise TypeError("Interval and only works on two intervals.")
			
		if not self.Overlaps(other):
			return Interval()
			
		start = max(self.a, other.a)
		end = min(self.b, other.b)
		
		return Interval(start, end)
	
	def __xor__(self, other):
		"""
		a ^ b : Returns the longest contiguous interval covered by a or b but not both.
		Returns an empty interval if they overlap perfectly.
		"""
		if not isinstance(other, Interval):
			raise TypeError("Interval xor only works on two intervals.")
		i = Interval()
		if self.a == other.a and self.b == other.b:
			return i
		
		# FIXME: Not complete.
		
	def __or__(self, other):
		"""
		a | b : Returns the longest contiguous interval that consists of either a or b or both.
		Returns an empty interval iff both a and b are empty.
		"""
		if not isinstance(other, Interval):
			raise TypeError("Interval or only works on two intervals.")
		i = Interval()
		if not self.Overlaps(other):
			if self.GetLength() > other.GetLength():
				i.a = self.a
				i.b = self.b
			else:
				i.a = other.a
				i.b = other.b
			return i
			
		if (self.a < other.a):
			start = self.a
		else:
			start = other.a
		
		if (self.b > other.b):
			end = self.b
		else:
			end = other.b
		
		i.SetRealInterval(start, end)
		return i
		
	def __iadd__(self, other):
		self.SetRealInterval(self.a+other.a, self.b+self.b)
		return self
		
	def __isub__(self, other):
		self.SetRealInterval(self.a-other.b, self.b-other.a)
		return self
		
	def __imul__(self, other):
		# FIXME: Unimplemented 
		raise NotImplemented
		
	def __idiv__(self, other):
		# FIXME: Unimplemented 
		raise NotImplemented
				
	def __ipow__(self, other):
		# FIXME: Unimplemented 
		raise NotImplemented
		
	def __ilshift__(self, other):
		if type(other) == int or type(other) == float:
			self.a -= other
			self.b -= other
		return self
		
	def __irshift__(self, other):
		if type(other) == int or type(other) == float:
			self.a += other
			self.b += other
		return self
		
	def __iand__(self, other):
		# FIXME: Unimplemented 
		raise NotImplemented
		
	def __ixor__(self, other):
		# FIXME: Unimplemented 
		raise NotImplemented
		
	def __ior__(self, other):
		# FIXME: Unimplemented 
		raise NotImplemented
	
	def __invert__(self):
		"""
		Mirrors the interval over the 0 point.
		"""
		i = Interval()
		i.b = -self.a
		i.a = -self.b
		return i

	def __str__(self):
		return "[%lf, %lf]" % (self.a, self.b)
		
	def __lt__(self, other):
		# FIXME: Unimplemented 
		raise NotImplemented
		
	def __le__(self, other):
		# FIXME: Unimplemented 
		raise NotImplemented
		
	def __eq__(self, other):
		return (self.a == other.a) and (self.b == other.b)
		
	def __ne__(self, other):
		return (self.a != other.a) or (self.b != other.b)
		
	def __gt__(self, other):
		# FIXME: Unimplemented 
		raise NotImplemented
		
	def __ge__(self, other):
		# FIXME: Unimplemented 
		raise NotImplemented
	
	def __hash__(self):
		# FIXME: Bad implementation
		return self.a + self.b + (self.a.__hash__() ^ self.b.__hash__()) + self.GetLength()

	class OutOfOrderException(Exception):
		def __init__(self):
			print "On a closed interval [a,b], a < b."
		
