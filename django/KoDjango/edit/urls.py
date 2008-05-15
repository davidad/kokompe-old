from django.conf.urls.defaults import *

urlpatterns = patterns('django.views.generic.simple',
	(r'^$','direct_to_template', {"template": "edit/index.html"}),
	(r'^Python/$','direct_to_template', {"template": "edit/python.html"}),
	(r'^MathString/$','direct_to_template', {"template": "edit/mathstring.html"}),
	(r'^GEE/$','direct_to_template', {"template": "edit/gee.html"}),
)

