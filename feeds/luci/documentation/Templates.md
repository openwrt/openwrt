LuCI has a simple regex based template processor which parses HTML-files to Lua functions and allows to store precompiled template files.
The simplest form of a template is just an ordinary HTML-file. It will be printed out to the user as is.

In LuCI every template is an object with an own scope. It can therefore be instantiated and each instance can has a different scope. As every template processor. LuCI supports several special markups. Those are enclosed in `<% %>`-Tags.

By adding `-` (dash) right after the opening `<%` every whitespace before the markup will be stripped. Adding a `-` right before the closing `%>` will equivalently strip every whitespace behind the markup.


# Builtin functions and markups
## Including Lua code
*Markup:*
	
	<% code %>
	


## Writing variables and function values
*Syntax:*
	
	<% write (value) %>
	

*Short-Markup:*
	
	<%=value%>
	

## Including templates
*Syntax:*
	
	<% include (templatename) %>
	

*Short-Markup:*
	
	<%+templatename%>
	


## Translating
*Syntax:*
	
	<%= translate("Text to translate") %>
	


*Short-Markup:*
	
	<%:Text to translate%>
	


## Commenting
*Markup:*
	
	<%# comment %>
	

# Builtin constants
| Name | Value |
---------|---------
|`REQUEST_URI`|The current URL (without server part)|
|`controller`|Path to the Luci main dispatcher|
|`resource`|Path to the resource directory|
|`media`|Path to the active theme directory|
