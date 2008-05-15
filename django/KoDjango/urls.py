from django.conf.urls.defaults import *

localstatic = "/home/spm/fablab/Kokompe/kokompe/django/Media/"

urlpatterns = patterns('',
	# Dynamic media:
	(r'^$', 'KoDjango.users.views.login'),
	(r'^Draw/', include('KoDjango.draw.urls')),
	(r'^Edit/', include('KoDjango.edit.urls')),
	(r'^Warehouse/$', 'KoDjango.warehouse.views.index'),
	(r'^Warehouse/(.*)$', 'KoDjango.warehouse.views.item'),
	(r'^About/', include('KoDjango.about.urls')),
	(r'^Service/', include('KoDjango.services.urls')),
	(r'^Services/', include('KoDjango.services.urls')),
	(r'^Users/', include('KoDjango.users.urls')),
	(r'^Admin/', include('django.contrib.admin.urls')),

	# OpenID:
	(r'^OpenID/$', 'KoDjango.users.openid_form'),
	(r'^OpenID/Process/(?P<token>.*)/$', 'KoDjango.users.openid_process'),


	# Static media:
	(r'^Styles/(.*)$', 'django.views.static.serve', {'document_root': localstatic+'Styles'}),
	(r'^Scripts/(.*)$', 'django.views.static.serve', {'document_root': localstatic+'Scripts'}),
	(r'^Images/(.*)$', 'django.views.static.serve', {'document_root': localstatic+'Images'}),
	(r'^Flash/(.*)$', 'django.views.static.serve', {'document_root': localstatic+'Flash'}),
	(r'^Java/(.*)$', 'django.views.static.serve', {'document_root': localstatic+'Java'}),
	(r'^SVG/(.*)$', 'django.views.static.serve', {'document_root': localstatic+'SVG'}),
	(r'^Temp/(.*)$', 'django.views.static.serve', {'document_root': localstatic+'Temp'}),
	(r'^Media/(.*)$', 'django.views.static.serve', {'document_root': localstatic}),
)
