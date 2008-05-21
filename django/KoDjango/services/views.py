from __future__ import division
import math
import Image, ImageDraw
from django.http import HttpResponse,HttpResponseRedirect
from django.contrib.auth.models import User
from django.contrib.auth import authenticate, login, logout

def service_login(request):
	user = authenticate(username=request.GET.get("username"), password=request.GET.get("password"))
	if user is not None:
		if user.is_active:
			login(request, user)
			return HttpResponse('{success:1}')
		else:
			return HttpResponse('{error:"The selected account has been disabled."}')
	else:
		return HttpResponse('{error:"Your username and password were incorrect."}')


def service_logout(request):
	logout(request)
	return HttpResponse('{success:1}')
	
def service_checkloggedin(request):
	if request.user.is_authenticated():
		return HttpResponse('{loggedin: true}')
	else:
		return HttpResponse('{loggedin: false}')

def service_rendermathstring(request):
	from os import popen2
	def execio(prog, inputstring):
		(input, output) = popen2("../Engine/" + prog);
		input.write(inputstring)
		input.close()
		outputstring = output.read()
		output.close()
		return outputstring
	
	mathstring = request.POST.get("mathstring", "(((((((0 + (X-0)/(0.75 + 0.5*cos(-1.57079632679 + (1.57079632679--1.57079632679)*(Z--0.5)/(0.5--0.5))))-0)**2 + ((0 + (Y-0)/(0.75 + 0.5*cos(-1.57079632679 + (1.57079632679--1.57079632679)*(Z--0.5)/(0.5--0.5))))-0)**2 <= 0.5**2) & (Z >= -0.5) & (Z <= 0.5)) | (((0.35 - sqrt(((Z-0)-0)**2 + ((0 + ((X--0.6)-0)/0.75)-0)**2))**2 + ((0 + ((Y-0)-0)/3)-0)**2) <= 0.05**2) | ((((0.55 - sqrt((X-1.05)**2 + (Z--0.1)**2))**2 + (Y-0)**2) <= 0.1**2)) & (((X >= -1.5) & (X <= 0.9) & (Y >= -1.5) & (Y <= 1.5) & (Z >= 0) & (Z <= 1.5)))) & ~(((((0 + (X-0)/(0.75 + 0.5*cos(-1.57079632679 + (1.57079632679--1.57079632679)*(Z--0.5)/(0.5--0.5))))-0)**2 + ((0 + (Y-0)/(0.75 + 0.5*cos(-1.57079632679 + (1.57079632679--1.57079632679)*(Z--0.5)/(0.5--0.5))))-0)**2 <= 0.4**2) & (Z >= -0.4) & (Z <= 0.6)))) & ~(((((0.55 - sqrt((X-1.05)**2 + (Z--0.1)**2))**2 + (Y-0)**2) <= 0.075**2)) & (((X >= -1.5) & (X <= 0.9) & (Y >= -1.5) & (Y <= 1.5) & (Z >= 0) & (Z <= 1.5))))) & ~(((X >= 0) & (X <= 1.5) & (Y >= -1.5) & (Y <= 0) & (Z >= -1.5) & (Z <= 1.5)))")
	print "Mathstring is '%s'." % mathstring
	dot_postfix = execio("infix_to_postfix", mathstring)
	dot_stl = execio("math_string_to_stl -1 1 -1 1 -1 1 0.05 2", dot_postfix)
	dot_ppm = execio("stl_to_ppm", dot_stl)
	import Image
	import StringIO
	im = Image.open(StringIO.StringIO(dot_ppm)).save("../Media/Temp/fab.jpg")

	return HttpResponse('<img src="/Media/Temp/fab.jpg"/>')



def polygon_addpoint_offset(poly, x, y, DPI=20):
	poly.append( ((poly[-1][0]+x)*DPI, (poly[-1][1]+y)*DPI) )
	return poly
	
def polygon_addpoint(poly, x, y, DPI=20):
	poly.append( (x*DPI, y*DPI) )
	return poly

def service_makegears(request):
	""" 
		Web app for making gears. Writes G
		
		Smari McCarthy <smarim@nmi.is>, 21. May 2008

		Based on gears.py by Carl Scheffler (3. March 2006)

		You may download, use and modify this software for any peaceful purpose
		whatsoever. The software remains the intellectual property of the author
		and he would appreciate it if you recognise and credit him as such. This
		software carries no guarantee and may well contain bugs.
	"""
	DPI = 300
	
	# Pitch of first gear
	pitch0 = float(request.GET.get("pitch0", "2"))
	# Number of gears
	num_gears = int(request.GET.get("count", "2"))
	# Teeth count
	num_teeth = map(int, request.GET.get("numteeth", "10,10").split(","))
	# Axis diameters
	shaft_radius = map(float, request.GET.get("shaftradius", "1,1").split(","))
	# Pressure angle
	pressure_angle = float(request.GET.get("pressureangle", 22))/ 180 * math.pi
	
	assert(len(num_teeth) == len(shaft_radius) == num_gears)
	
	commands = []
	polygons = []

	gears = [{'n_teeth': num_teeth[0], 'pitch_r': pitch0, 'shaft_r': shaft_radius[0]}]
	for i in range(1, num_gears):
		gears.append({'n_teeth': num_teeth[i],
					  'vel_ratio': num_teeth[i]/num_teeth[i-1],
					  'pitch_r': num_teeth[i]/num_teeth[i-1] * gears[i-1]['pitch_r'],
					  'shaft_r': shaft_radius[i]
					})
		
	for gear in range(len(gears)):
		# Pitch angle (angular width of tooth and space)
		gears[gear]['pitch_ang'] = 2*math.pi / gears[gear]['n_teeth']
		# Module (equal for the 2 gears)
		gears[gear]['module'] = 2 * gears[gear]['pitch_r'] / gears[gear]['n_teeth']
		# Addendum, dedendum
		gears[gear]['addendum'] = 0.45 * gears[gear]['module']
		gears[gear]['addendum_r'] = gears[gear]['pitch_r'] + gears[gear]['addendum']
		gears[gear]['dedendum'] = 1.0 * gears[gear]['module']
		gears[gear]['dedendum_r'] = gears[gear]['pitch_r'] - gears[gear]['dedendum']
		# Radii of the base circles
		gears[gear]['base_r'] = gears[gear]['pitch_r'] * math.cos(pressure_angle)

	for gear in range(len(gears)):
		# Curve parameters for gear
		# Angles at which involute intersects base, pitch and addendum circles
		a_add = involute_a_of_r(gears[gear]['base_r'], gears[gear]['addendum_r'])
		theta = {}
		theta['base_left']	= 0
		theta['pitch_left']	= involute_theta_of_r(gears[gear]['base_r'], gears[gear]['pitch_r'])
		theta['add_left']	= involute_theta_of_a(a_add)
		theta['pitch_right']	= theta['pitch_left'] + gears[gear]['pitch_ang'] / 2
		theta['add_right']	= theta['pitch_right'] - (theta['add_left'] - theta['pitch_left'])
		theta['base_right']	= theta['pitch_right'] + theta['pitch_left']

		# Check and correct if addendum circle not touched
		if theta['add_left'] > theta['pitch_left'] + gears[gear]['pitch_ang'] / 4:
			print theta['add_left'], theta['pitch_left'] + gears[gear]['pitch_ang'] / 4
			assert False, 'crap: ' + str(gear)

		curve = []											   # (r, theta) coordinates
		curve.append([gears[gear]['base_r'], 0])				 # point on base circle
		# build involute curves
		N = 20
		curve_involute = []
		for i in range(1, N+1):
			a = a_add * i / N
			curve_involute.append([involute_r_of_a(gears[gear]['base_r'], a),
								   involute_theta_of_a(a)])
		curve += curve_involute									# left involute
		for i in range(1, N):									  # addendum circle arc
			curve.append([gears[gear]['addendum_r'],
						  ((N-i)*theta['add_left'] + i*theta['add_right'])/N])
		curve_involute.reverse()								   # right involute
		for rtheta in curve_involute:
			curve.append([rtheta[0], theta['add_left'] - rtheta[1] + theta['add_right']])
		curve.append([gears[gear]['base_r'], theta['base_right']]) # point on base circle
		for i in range(N+1):									   # dedendum circle arc
			curve.append([gears[gear]['dedendum_r'],
						  ((N-i)*theta['base_right'] + i*gears[gear]['pitch_ang'])/N])

		# Draw the gear
		if gear == 0.:
			dy = 12 - 4.5 - gears[gear]['addendum']
		else:
			#dx += gears[gear-1]['addendum_r'] + gears[gear]['addendum_r'] + 0.25
			dy -= gears[gear]['addendum_r'] + gears[gear-1]['addendum_r'] + 0.25
		dx = 9.5 + gear * (2*gears[gear]['addendum_r'] + 0.25)
		dy = 12 - 1.

		
		commands += ['PU', 'POS', dx+shaft_radius[gear], dy, 'PD']
		poly = polygon_addpoint([], dx+shaft_radius[gear], dy)
		for i in range(1, N+1):
			commands += ['POS',
						 gears[gear]['shaft_r'] * math.cos(i/N*2*math.pi) + dx,
						 gears[gear]['shaft_r'] * math.sin(i/N*2*math.pi) + dy]
			poly = polygon_addpoint(poly, gears[gear]['shaft_r'] * math.cos(i/N*2*math.pi) + dx, gears[gear]['shaft_r'] * math.sin(i/N*2*math.pi) + dy)
			
		polygons.append(poly)
		
		commands += ['PU', 'POS', dx+gears[gear]['dedendum_r'], dy, 'PD']
		poly = polygon_addpoint([], dx+gears[gear]['dedendum_r'], dy)
		
		for i in range(gears[gear]['n_teeth']):
			ang = 2*math.pi*i/gears[gear]['n_teeth']
			for rtheta in curve:
				commands += ['POS',
							 rtheta[0]*math.cos(ang + rtheta[1]) + dx,
							 rtheta[0]*math.sin(ang + rtheta[1]) + dy]
				poly = polygon_addpoint(poly, rtheta[0]*math.cos(ang + rtheta[1]) + dx, rtheta[0]*math.sin(ang + rtheta[1]) + dy)
				
		polygons.append(poly)

	image = Image.new("RGB", (800, 800), color=0xFFFFFF)

	draw = ImageDraw.Draw(image)
	for poly in polygons:
		draw.polygon(poly, outline=0xFF000000)
		
	del draw
	
	image.save("../Media/Temp/gears.png")
	
	return HttpResponse('<img src="/Media/Temp/gears.png"/>')
	#myturtle = FabTurtle(commands)
	#myturtle.write_epi('gear.epi', rate=500, power=10, speed=1,
	#				   xoff = 0.,
	#				   yoff = 0.)
	#window = simulate_epi('gear.epi')
	#window.wait_window()


def DegreesToRadians(deg):
	return deg / 180 * math.pi

def involute_r_of_a(base_r, a):
	return base_r * math.sqrt(1+a**2)

def involute_theta_of_a(a):
	return math.atan2(math.tan(a)-a, 1-a*math.tan(a))

def involute_a_of_r(base_r, r):
	return math.sqrt((r/base_r)**2-1)

def involute_theta_of_r(base_r, r):
	return involute_theta_of_a(involute_a_of_r(base_r, r))
