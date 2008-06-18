from django.db import models
from django.contrib.auth.models import User
from KoDjango.users.models import Lab

# Create your models here.

class InventoryItem(models.Model):
	
	Name = models.CharField(maxlength=200, help_text="Please supply the item with a unique name.")
	Description = models.TextField(blank=True,null=True,help_text="Please describe the item in detail.")
	
	OrderNumber = models.CharField(maxlength=70, help_text="Ordering number for vendor.")
	Vendor = models.CharField(maxlength=70, help_text="Vendor name.")
	OrderUnitPrice = models.FloatField(help_text="What the lab pays for each item.")
	
	class Admin:
		pass
	
	def __unicode__(self):
		return self.Name
		
		
class Inventory(models.Model):
	Lab = models.ForeignKey(Lab, help_text="The Fab Lab.")
	Item = models.ForeignKey(InventoryItem, help_text="The item.")
	Count = models.IntegerField(help_text="How many exist in the lab inventory.")
	UnitPrice = models.FloatField(help_text="What people buy each item from the lab for.")

	class Meta:
		verbose_name_plural="Inventories"

	class Admin:
		pass
	
	def __unicode__(self):
		return "%s - %d" % (self.Item.Name, self.Count)
		

class UsedItems(models.Model):
	User = models.ForeignKey(User, help_text="Who used the item")
	Item = models.ForeignKey(InventoryItem, help_text="What item was used")
	Count = models.IntegerField(help_text = "How many of this item were used")
	
	class Meta:
		verbose_name_plural="Used items"

	class Admin:
		pass