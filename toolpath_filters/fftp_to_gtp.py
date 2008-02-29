import sys
from struct import pack

def FFTP2GTP(fftp, out):
	"""
	Given a FFTP input (either a list of strings with one set of curly braces per string or a file providing the same) and 
	a binary writable output file, this function will dump pure headerless almost feature free GTP binary code into the
	file in binary format.
	"""
	scale = 100
	for a in fftp:
		b = [[float(p) for p in m.split(", ") if p != ""] for m in a.strip("{}[]\n\t ").split("], [") if len(m) != 0]
		if len(b) == 0:
			continue
		out.write(pack(">LLL", int(b[0][0] * scale), int(b[0][1] * scale), 0xFFFF0013))
		b = b[2:]
		for x in b:
			out.write(pack(">LLL", int(x[0] * scale), int(x[1] * scale), 0xFFFF0012))

if __name__ == "__main__":
	if len(sys.argv) > 2:
		FFTP2GTP(open(sys.argv[1]), open(sys.argv[2], "wb"))
	else:
		print "Usage as a standalone program: python fftp_to_gtp.py input.toolpath output.gtp"
		print "The output file must be writable."
