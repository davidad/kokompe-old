from django.template import RequestContext,loader
from django.http import HttpResponse,HttpResponseRedirect

def index(request):
	return HttpResponse(loader.get_template('warehouse/index.html').render(RequestContext(request)))

