 Design of the `cli` module API

## Structure:
The cli is organized as a set of *nodes*, which are ucode objects objects describing *entries*.
Each *entry* can either implement a *command*, or select another *node*, optionally with *parameters*.
Additionally, it contains helptext and full *parameter* descriptions, including everything needed for tab completion.
The initial *node* on startup is `Root`, representing the main menu.

## Simple example:

### Code:
```
const Example = {
	hello: {
		help: "Example command",
		args: [
			{
				name: "name",
				type: "string",
				min: 3,
				max: 16,
				required: true,
			}
		],
		call: function(ctx, argv, named) {
			return ctx.ok("Hello, " + argv[0]);
		},
	},
	hello2: {
		help: "Example command (named_args version)",
		named_args: {
			name: {
				required: true,
				args: {
					type: "string",
					min: 3,
					max: 16,
				}
			}
		},
		call: function(ctx, argv, named) {
			return ctx.ok("Hello, " + named.name);
		},
	}
};

const Root = {
	example: {
		help: "Example node",
		select_node: "Example",
	}
};

model.add_nodes({ Root, Example });
```
### Example interaction:
```
root@OpenWrt:~# cli
Welcome to the OpenWrt CLI. Press '?' for help on commands/arguments
cli> example 
cli example> hello
Error: Missing argument 1: name
cli example> hello foo
Hello, foo
cli example> hello2
Error: Missing argument: name
cli example> hello2 name foo2
Hello, foo2
cli example> 
```

## API documentation:

Each module is placed in `/usr/share/ucode/cli/modules` on the root filesystem.
When included by the cli code, the scope contains the `model` variable, which is the main cli API object. This variable is also present in the scope of the other callback functions described below.

### `model` methods:
- `model.warn(msg)`: Pass a warning to the user (similar to the ucode `warn` function).
- `model.exception(e)`: Print an exception with stack trace.
- `model.add_module(path)`: Load a single module from `path`
- `model.add_modules(path)`: Load multiple modules from the `path` wildcard pattern
- `model.add_node(name, obj)`: Add a single node under the given name
- `model.add_nodes(nodes)`: Add multiple nodes with taking `name` and `obj` from the `nodes` object.
- `model.add_type(name, info)`: Add a data type with validation information,
- `model.add_types(types)`: Add multiple data types, taking `name` and `info` from the `types` object.
- `model.status_msg(msg)`: Print an asynchronous status message (should not be used from within a node `call` or `select` function).

### Properties of an `entry` inside a `node`:
Each entry must have at least `help` and either `call` or `select_node` set. 
- `help`: Helptext describing the command
- `call: function(ctx, argv, named)`: main command handler function of the entry.
	- `this`: pointer to the `entry`
	- `ctx`: call context object (see below)
	- `argv`: array of positional arguments after the command name
	- `named`: object of named parameters passed to the command
	- Return value: either `ctx.ok(msg)` for successfull calls, or the result of an error function (see below).
- `select_node`: (string) name of the *node* that this entry points to. Mutually exclusive with implementing `call`.
- `select: function(ctx, argv, named)`: function for selecting another node.
	- `this`: pointer to the *entry*
	- `ctx`: node context object (see below)
	- `argv`, `named`: see `call`
	- Return value: either `ctx.set(prompt, data)`, `true`, or the result of an error function (see below).
- `args`: array of positional *arguments* (see *argument* property description)
- `named_args`: object of named *parameters*  (see *parameter* property description)
- `available: function(ctx)`: function indicating if the entry can be used (affects tab completion and running commands)
	- `this`: pointer to the *entry*
	- `ctx`: node context object (see below)
	- Return value: `true` if available, `false` otherwise.
- `validate: function (ctx, argv, named)`: validate command arguments
	- Function parameters: see `call`

### Named *parameter* properties:
- `help`: Description of the named parameter's purpose
- `args`: Either an array of *argument* objects, or an object with a single *argument* (see below). If not set, paramter will not take any arguments, and its value will be `true` if the parameter was specified on the command line.
- `available: function(ctx, argv, named)`: function indicating if the named parameter can be used (affects tab completion and argument validation). May depend on *arguments*/*parameters* specified before this one.
- `multiple` (bool): indicates if an argument may be specified multiple times. Turns the value in `named` into an array.
- `required` (bool): Parameter must be specified for the command
- `default`: default value for the parameter.
- `allow_empty`: empty values are allowed and can be specified on the command line using `-param_name` instead of `param_name`. The value in the `named` object will be `null` in that case.

### Positional *argument* properties:
- `name`: Short name of the *argument*
- `help`: Longer description of the *argument* (used in helptext/completion)
- `type`: data type name (see below)
- `required` (bool): Value must not be empty
- `value`: possible values for tab completion, one of:
	- array of objects with the following contents:
		- `name`: value string
		- `help`: help text for this value
	- `function(ctx, argv, named)` returning the above.
- extra properties specific to the data type (see below)

### Default data types:
- `int`: Integer value. The valid range can be specified using the `min` and `max` properties.
- `string`: String value. The valid string length can be specified using the `min` and `max` properties.
- `bool`: Boolean value. Converts `"1"` and `"0"` to `true` and `false`
- `enum`: String value that must match one entry of the list provided via the `value` property. Case-insensitive match can be enabled using the `ignore_case` property.
- `path`: Local filesystem path. When the `new_path` property is set, only match directories for a file to be created.
- `host`: Host name or IP address
- `macaddr`: MAC address
- `ipv4`: IPv4 address
- `ipv6`: IPv6 address
- `cidr4`: IPv4 address with netmask size, e.g. 192.168.1.1/24. Allows `auto` as value if the `allow_auto` property is set.

### `call` context:
Passed as `ctx` argument to entry `call` functions.
- `ctx.data`: Object containing any data passed via `ctx.set()` from a `select` context.
- `ctx.ok(msg)`: Indicates successful call, passes the message `msg` to the user.
- `ctx.select(...args)`: After completion, switch to a different *node* by running the command chain provided as function argument (only entries with `.select_node` are supported).
- `ctx.string(name, val)`: Passes a string to the caller as return value.
- `ctx.list(name, val)`: Passes a list of values to the caller as return value. `val` must be an array.
- `ctx.table(name, val)`: Passes a table as value to the caller. `val` can be an array `[ column_1, column_2 ]`, where each member of the outer array describes a row in the table. It can also be an object, where the property name is the first column value, and the value the second column value.
- `ctx.multi_table(name, val)`: Passes multiple tables to the caller. Can be an array of `[ title, table ]`, or an object.
- Error functions (see below)

### `select` context:
- `ctx.data`: Object containing any data passed via parent `ctx.set` calls.
- `ctx.set(prompt, data)`: Modify the prompt and `ctx.data` for the child context. The string given in `prompt` is appended to the existing prompt. The data given in the `data` object is merged with the previous `ctx.data` value.
- Error functions (see below)

### Error functions:
All error messages accept a format string in `msg`, with arguments added after it.
- `ctx.invalid_argument(msg, ...args)`: Indicates that invalid arguments were provided.
- `ctx.missing_argument(msg, ...args)`: Indicates that an expected argument was missing.
- `ctx.command_failed(msg, ...args)`: Indicates that the command failed.
- `ctx.not_found(msg, ...args)`: Indicates that a given entry was not found.
- `ctx.unknown_error(msg, ...args)`: Indicates that the command failed for unknown or unspecified reasons.
- `ctx.error(id, msg, ...args)`: Generic error message with `id` specifying a machine readable error type string.

## Editor API documentation
The editor API provides a layer of abstraction above node entries/calls in order to make it easy to edit properties of an object based on an attribute list, as well as create/destroy/show object instances using a consistent user interface.

### Simple example:
```
import * as editor from "cli.object-editor";

let changed = false;
let data = {
	things: {
		foo: {
			label_str: [ "bar" ],
			id: 31337,
		}
	},
};

const thing_editor = {
	change_cb: function(ctx) {
		changed = true;
	},
	named_args: {
		label: {
			help: "Thing label",
			attribute: "label_str",
			multiple: true,
			args: {
				type: "string",
				min: 2,
				max: 16
			},
		},
		id: {
			help: "Thing id",
			required: true,
			args: {
				type: "int",
				min: 1,
			},
		},
	},
};
const ExampleThing = editor.new(thing_editor);

let Example = {
	dump: {
		help: "Dump current data",
		call: function(ctx, argv, named) {
			return ctx.json("Data", {
				changed,
				data
			});
		},
	}
};
const example_editor = {
	change_cb: function(ctx) {
		changed = true;
	},
	types: {
		thing: {
			node_name: "ExampleThing",
			node: ExampleThing,
			object: "things",
		},
	},
};
editor.edit_create_destroy(example_editor, Example);

const Root = {
	example: {
		help: "Example node",
		select_node: "Example",
		select: function(ctx, argv, named) {
			return ctx.set(null, {
				object_edit: data,
			});
		}
	}
};

model.add_nodes({ Root, Example, ExampleThing });
```
### Example interaction:
```
root@OpenWrt:~# cli
Welcome to the OpenWrt CLI. Press '?' for help on commands/arguments
cli> example
cli example> dump
Data: {
        "changed": false,
        "data": {
                "things": {
                        "foo": {
                                "label_str": [
                                        "bar"
                                ],
                                "id": 31337
                        }
                }
        }
}
cli example> thing foo set id 1337
cli example> create thing bar id 168 label l1 label l2
Added thing 'bar'
cli example> thing bar show
Values: 
    id:         168
    label:      l1, l2
cli example> thing bar remove label 1
cli example> thing bar show
Values: 
    id:         168
    label:      l2
cli example> dump
Data: {
        "changed": true,
        "data": {
                "things": {
                        "foo": {
                                "label_str": [
                                        "bar"
                                ],
                                "id": 1337
                        },
                        "bar": {
                                "id": 168,
                                "label_str": [
                                        "l2"
                                ]
                        }
                }
        }
}
cli example> destroy thing foo 
Deleted thing 'foo'
cli example> 
```
### API documentation
Prelude: `import * as editor from "cli.object-editor";`

#### Object editor:
For editing an object, the following user commands are defined:
- `set`: Changes property values
- `show` Show all values

If properties with `multiple: true` are defined, the following commands are also defined:
- `add`: Add values to properties
- `remove` Remove specific values from properties

##### Variant 1 (editor-only node):
`const Node = editor.new(editor_data)`

##### Variant 2 (merge with existing entries):
`let Node = {};`
`editor.new(editor_data, Node);`

The editor code assumes that the *node* that selects the editor node uses `ctx.set()` to set the `edit` field in `ctx.data` to the object being edited.

#### `editor_data` properties:
- `change_cb: function(ctx)`: Called whenever a property is changed by the user
- `named_args`: Parameters for editing properties (based on *entry* `named_args`, see below)
- `add`, `set`, `show`, `remove`: Object for overriding fields of the commands defined by the editor. Primarily used to override the helptext.

#### Instance editor `named_args` entry properties:
All *entry* `named_args` properties are supported, but the meaning is extended slightly:
- `multiple`: Property array values can be added/removed
- `default`: Default value when creating the object
- `allow_empty`: Property can be deleted
- `required`: Property is mandatory in the object.

#### Object instance editor:
For managing object instances, the following user commands are defined:
- `create <type> <name> <...>`: Create a new instance. Also takes parameter values to be set on the object.
- `destroy <type> <name>`: Delete an instance.
- `list <type>` List all instances of a given type.

The instance editor code assumes that the *node* that selects the editor node uses `ctx.set()` to set the `object_edit` field in `ctx.data` to the object being edited.

##### Variant 1 (editor-only node):
`const Node = editor.edit_create_destroy(instance_data);`

##### Variant 2 (merge with existing entries):
`let Node = {};`
`editor.edit_create_destroy(instance_data, Node);`

#### `instance_data` properties:
- `change_cb: function(ctx)`: Called whenever an instance is added or deleted
- `types`: Metadata about instances types (see below)

#### `instance_data.types` object properties:
- `node_name`: name of the *editor node* belonging to the object instance.
- `node`: The *editor node* itself.
- `object`: Name of the type specific container object inside the object pointed to by `ctx.data.object_edit`. 

