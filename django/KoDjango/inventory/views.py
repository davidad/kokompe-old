from django.http import HttpResponse,HttpResponseRedirect
from django.views.generic.simple import direct_to_template
from models import Inventory, Lab

def InventoryIndex(request):
	lab = Lab.objects.filter()[0]
	inventory = Inventory.objects.filter(Lab=lab)
	
	context = {'lab': lab, 'inventory': inventory }
	
	return direct_to_template(request, 'inventory/index.html', extra_context=context)

	
def InventoryAdd(request):
	return HttpResponse()


def InventoryRemove(request):
	return HttpResponse()

