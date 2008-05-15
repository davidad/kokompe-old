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
		(input, output) = popen2("/home/spm/fablab/Kokompe/kokompe/engine/" + prog);
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
	im = Image.open(StringIO.StringIO(dot_ppm)).save("/home/spm/fablab/Kokompe/kokompe/django/Media/Temp/fab.jpg")

	return HttpResponse('<img src="/Media/Temp/fab.jpg"/>')
