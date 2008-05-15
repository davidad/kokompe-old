from django.conf.urls.defaults import *

urlpatterns = patterns('django.views.generic.simple',
	(r'^$','direct_to_template', {"template": "about/index.html"}),
	(r'^Privacy/$','direct_to_template', {"template": "about/privacy.html"}),
	(r'^Copyright/$','direct_to_template', {"template": "about/copyright.html"}),
	(r'^Advertising/$','direct_to_template', {"template": "about/advertising.html"}),
	(r'^SubmissionGuidelines/$','direct_to_template', {"template": "about/submission_guidelines.html"}),
	(r'^WarehouseGuidelines/$','direct_to_template', {"template": "about/warehouse_guidelines.html"}),
	(r'^FabLabs/$','direct_to_template', {"template": "about/warehouse_guidelines.html"}),
)

