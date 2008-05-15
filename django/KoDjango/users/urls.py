from django.conf.urls.defaults import *

urlpatterns = patterns('TBDjango.users.views',
	(r'^$','index'),
	(r'^Login/$','login'),
	(r'^Settings/$', 'settings'),
)

