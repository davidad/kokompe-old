from django.contrib.auth.decorators import user_passes_test

staff_only = user_passes_test(lambda u: u.is_staff())
superusers_only = user_passes_test(lambda u: u.is_superuser())
