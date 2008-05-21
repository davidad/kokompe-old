from django.conf.urls.defaults import *

urlpatterns = patterns('KoDjango.services.views',
	(r'^Login/$','service_login'),
	(r'^Logout/$','service_logout'),
	(r'^CheckLoggedIn/$', 'service_checkloggedin'),
	(r'^RenderMathString/$', 'service_rendermathstring'),
	(r'^RenderGears/$', 'service_makegears'),
)

