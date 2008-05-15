

function UserManager(loginbtn, logindialog, login_user, login_pass, login_submit, login_cancel) {
	this.loginButton = loginbtn;
	this.loginDialog = logindialog;
	this.loginUsername = login_user;
	this.loginPassword = login_pass;
	this.loginSubmit = login_submit;
	this.loginCancel = login_cancel;
	this.loggedIn = false;
	this.username = "";
	this.password = "";	// We should never keep this for extended periods of time
	var t = this;
	this.loginButton.click(function(){ t.showLoginDialog(); });
	this.loginSubmit.click(function(){ t.login(); });
	this.loginCancel.click(function(){ t.hideLoginDialog(); });
	this.noticeDiv = $(document.createElement("DIV"));
	this.noticeDiv.addClass("notice");
	this.noticeDiv.text("&nbsp;");
	this.loginDialog.prepend(this.noticeDiv);
	this.checkLoggedIn();
	
	// this.loginDialog.dialog();
}


UserManager.prototype = {
	showLoginDialog:function() {
		this.loginDialog.fadeIn("slow");
	},
	
	hideLoginDialog:function() {
		this.loginDialog.fadeOut("slow");
	},
	
	login:function() {
		this.username = this.loginUsername.val();
		this.password = this.loginPassword.val();
		var t = this;
		$.getJSON("/Service/Login", {'username': this.username, 'password': this.password}, function(data) {
			if (data.error) {
				t.notice("Error: " + data.error);
			} else {
				t.loginButton.unbind("click");
				t.loginButton.click(function(){ t.logout(); });
				t.hideLoginDialog();
				t.loginButton.text("Logout");
			}
		});
	},
	
	logout:function() {
		var t = this;
		$.getJSON("/Service/Logout", function(data) {
			if (data.error) {
				alert("Error: " + data.error + "\nThis should never happen.");
			} else {
				t.loginButton.unbind("click");
				t.loginButton.click(function(){ t.showLoginDialog(); });
				t.loginButton.text("Login");
			}
		});
	},
	
	checkLoggedIn:function() {
		var t = this;
		$.getJSON("/Service/CheckLoggedIn", function(data) {
			if (data.loggedin == true) {
				t.loginButton.unbind("click");
				t.loginButton.click(function(){ t.logout(); });
				t.loginButton.text("Logout");
				t.loggedIn = true;
			} else {
				t.loginButton.unbind("click");
				t.loginButton.click(function(){ t.showLoginDialog(); });
				t.loginButton.text("Login");
				t.loggedIn = false;
			}
		});
	},
	
	notice:function(msg, noanim) {
		this.noticeDiv.text(msg)
		if (!noanim) {
			this.noticeDiv.stop().stop().animate({opacity:1.0}, 1000).animate({opacity:0}, 5000);
		} else {
			this.noticeDiv.stop().stop().animate({opacity:1.0}, 1000)
		}

	}
};