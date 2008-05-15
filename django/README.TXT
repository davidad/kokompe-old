Kokompe Django Web UI
=====================
Smári McCarthy <smari@fablab.is>

based on Kokompe Web IDE by Amy Sun <amy.sun@cba.mit.edu>


== Install ==
You'll need a few things:

  * Python >= 2.5
  * Django
  * A database:
	* MySQL is best and preferred for any larger installations.
	* SqlLite3 is tiny and provides a self-contained, serverless, 
		zero-configuration, transactional SQL database engine.
	* Postgre is also an option.
	* Whichever you use, edit KoDjango/settings.py and configure 
		appropriately.

== Run ==

  * Standalone:
	Enter KoDjango/ and type:
		$ python manage.py runserver 0.0.0.0:8000
	Exchange '8000' with the port number you wish to run the server on. 
	If using ports < 1024 you need to be root.
	Port 80 is the standard HTTP port. If no port is given, 8000 is 
	  used by default.
	0.0.0.0 means listen on all devices. If you don't give an IP address 
	  the server will run on localhost only (127.0.0.1) by default.

  * Apache:
	A bit harder to do. See documentation on Django web page:
		http://www.djangoproject.org

	The bulk of it is just:

		apt-get install apache libapache2-mod-python python-django
		

== Edit ==
Django uses a MVC architecture - Model, View, Controller.

Models are basic data structures that are exposed to the database or to a file.
	Example models are "User", "Project" and "File".
Views are functions that recieve HTTP requests and do computation on 
	requests and Models.
Controllers are represented by templates in Django. A template is more or less 
	just a file that contains variables that Views then fill into and 
	return to the user. The user then performs client side logic on the 
	controller to do another iteration.

"Kokompe" is considered to be a Django project, and contains a number of Django 
apps. Each app contains its own collection of models, views and controllers, and
the project takes care of dispatching each appropriately. Models, views and 
controllers can be shared between multiple apps, by having one app import from
another.

The directory structure is organized like so:

	Media/		- Static media files, such as javascript, images, etc.
			  This is stuff that Python should never attempt to
			  process.
	KoDjango/	- Main project directory
	  about/	- App with online help for Kokompe users.
	  draw/		- App for point and click drawing tools.
	  edit/		- App for text input programming tools.
	  services/	- App with various AJAX services.
	  users/	- App for user account management.
	  templates/	- Base controller templates.
	  warehouse/	- App for project management and display.

Each app directory contains the files "models.py" and "views.py". Additionally 
some contain "urls.py" and other files. "urls.py" is an URL mapping which is
referenced by the base "urls.py" in the KoDjango/ directory. 

Unlike PHP, files and functionality are not exposed on a file-by-file basis
to the web, but are rather selectively exposed through the URL map. This is
more secure, easier to maintain, and in the long run far less prone to clutter.


