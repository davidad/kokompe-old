from django.conf.urls.defaults import *

urlpatterns = patterns('KoDjango.inventory.views',
	(r'^$','InventoryIndex'),
	(r'^Detail/(.*)$','InventoryDetail'),
	(r'^Add/$','InventoryAdd'),
	(r'^Remove/$','InventoryRemove'),
)
