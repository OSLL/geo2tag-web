package ru.spb.osll.web.client.localization;

import com.google.gwt.i18n.client.Constants;

public interface GTLocalization extends Constants {
	String login();
	String password();
	String confirm();
	String registration();

	// buttons
	String btnOk();
	String btnCancel();
	String btnSignin();
	
	// warnings
	String wrngLoginNull();
	String wrngPasswordNull();
	String wrngConfirmNull();
	String wrngPassAndConf(); 
}