/**
* =============================================================================
* Source Python
* Copyright (C) 2012-2019 Source Python Development Team.  All rights reserved.
* =============================================================================
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License, version 3.0, as published by the
* Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
* As a special exception, the Source Python Team gives you permission
* to link the code of this program (as well as its derivative works) to
* "Half-Life 2," the "Source Engine," and any Game MODs that run on software
* by the Valve Corporation.  You must obey the GNU General Public License in
* all respects for all other code used.  Additionally, the Source.Python
* Development Team grants this exception to all derivative works.
*/

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "export_main.h"
#include "sp_main.h"
#include "core_cache.h"


//-----------------------------------------------------------------------------
// Forward declarations.
//-----------------------------------------------------------------------------
static void export_cached_property(scope);
static void export_cached_generator(scope);


//-----------------------------------------------------------------------------
// Declare the _core._cache module.
//-----------------------------------------------------------------------------
DECLARE_SP_SUBMODULE(_core, _cache)
{
	export_cached_property(_cache);
	export_cached_generator(_cache);
}


//-----------------------------------------------------------------------------
// Exports CachedProperty.
//-----------------------------------------------------------------------------
void export_cached_property(scope _cache)
{
	class_<CCachedProperty, CCachedProperty *> CachedProperty(
		"CachedProperty",
		init<object, object, object, const char *, boost::python::tuple, dict>(
			(
				arg("fget")=object(), arg("fset")=object(), arg("fdel")=object(), arg("doc")=object(),
				arg("args")=boost::python::tuple(), arg("kwargs")=dict()
			),
			"Represents a property attribute that is only"
			" computed once and cached.\n"
			"\n"
			":param function fget:\n"
			"	Optional getter function.\n"
			"	Once the value has been computed, the result is cached and"
			" returned if this property is requested again.\n"
			"\n"
			"	Getter signature: self, *args, **kwargs\n"
			":param function fset:\n"
			"	Optional setter function.\n"
			"	When a new value is assigned to this property, that"
			" function is called and the cache is invalidated if nothing"
			" was returned otherwise the cached value is updated accordingly.\n"
			"\n"
			"	Setter signature: self, value, *args, **kwargs\n"
			":param function fdel:\n"
			"	Optional deleter function.\n"
			"	When this property is deleted, that function is called and the"
			" cached value (if any) is invalidated.\n"
			"\n"
			"	Deleter signature: self, *args, **kwargs\n"
			":param str doc:\n"
			"	Documentation string for this property.\n"
			":param tuple args:\n"
			"	Extra arguments passed to the getter, setter and deleter functions.\n"
			":param dict kwargs:\n"
			"	Extra keyword arguments passed to the getter, setter and deleter functions.\n"
			"\n"
			":raises TypeError:\n"
			"	If the given getter, setter or deleter is not callable."
		)
	);


	CachedProperty.def(
		"getter",
		&CCachedProperty::set_getter,
		"Decorator used to register the getter function for this property.\n"
		"\n"
		":param function fget:\n"
		"	The function to register as getter function.\n"
		"\n"
		":rtype:\n"
		"	function"
	);

	CachedProperty.add_property(
		"fget",
		&CCachedProperty::get_getter,
		&CCachedProperty::set_getter,
		"The getter function used to compute the value of this property.\n"
		"\n"
		":rtype:\n"
		"	function"
	);


	CachedProperty.def(
		"setter",
		&CCachedProperty::set_setter,
		"Decorator used to register the setter function for this property.\n"
		"\n"
		":param function fset:\n"
		"	The function to register as setter function.\n"
		"\n"
		":rtype:\n"
		"	function"
	);

	CachedProperty.add_property(
		"fset",
		&CCachedProperty::get_setter,
		&CCachedProperty::set_setter,
		"The setter function used to assign the value of this property.\n"
		"\n"
		":rtype:\n"
		"	function"
	);


	CachedProperty.def(
		"deleter",
		&CCachedProperty::set_deleter,
		"Decorator used to register the deleter function for this property.\n"
		"\n"
		":param function fdel:\n"
		"	The function to register as deleter function.\n"
		"\n"
		":rtype:\n"
		"	function"
	);

	CachedProperty.add_property(
		"fdel",
		&CCachedProperty::get_deleter,
		&CCachedProperty::set_deleter,
		"The deleter function used to delete this property.\n"
		"\n"
		":rtype:\n"
		"	function"
	);


	CachedProperty.def_readwrite(
		"__doc__",
		&CCachedProperty::m_szDocString,
		"Documentation string for this property.\n"
		"\n"
		":rtype:\n"
		"	str"
	);


	CachedProperty.add_property(
		"owner",
		&CCachedProperty::get_owner,
		"The owner class this property attribute was bound to.\n"
		"\n"
		":rtype:\n"
		"	type"
	);

	CachedProperty.add_property(
		"name",
		&CCachedProperty::get_name,
		"The name this property is registered as.\n"
		"\n"
		":rtype:\n"
		"	str"
	);

	CachedProperty.def_readwrite(
		"args",
		&CCachedProperty::m_args,
		"The extra arguments passed to the getter, setter and deleter functions."
	);

	CachedProperty.def_readwrite(
		"kwargs",
		&CCachedProperty::m_kwargs,
		"The extra keyword arguments passed to the getter, setter and deleter functions."
	);

	CachedProperty.def(
		"__set_name__",
		&CCachedProperty::__set_name__,
		"Called when this property is being bound to a class.\n"
	);

	CachedProperty.def(
		"__get__",
		&CCachedProperty::__get__,
		"Retrieves the value of this property.\n"
		"\n"
		":rtype:\n"
		"	object"
	);

	CachedProperty.def(
		"__set__",
		&CCachedProperty::__set__,
		"Assigns the value of this property.\n"
		"\n"
		":rtype:\n"
		"	object"
	);

	CachedProperty.def(
		"__delete__",
		&CCachedProperty::__delete__,
		"Deletes this property and invalidates its cached value."
	);

	CachedProperty.def(
		"__call__",
		&CCachedProperty::set_getter,
		"Decorator used to register the getter function for this property.\n"
		"\n"
		":param function fget:\n"
		"	The function to register as getter function.\n"
		"\n"
		":rtype:\n"
		"	function"
	);

	CachedProperty.def(
		"__getitem__",
		&CCachedProperty::__getitem__,
		"Returns the value of a keyword argument.\n"
		"\n"
		":param str item:\n"
		"	The name of the keyword.\n"
		"\n"
		":rtype:"
		"	object"
	);

	CachedProperty.def(
		"__setitem__",
		&CCachedProperty::__setitem__,
		"Sets the value of a keyword argument.\n"
		"\n"
		":param str item:\n"
		"	The name of the keyword.\n"
		":param object value:\n"
		"	The value to assigne to the given keyword."
	);

	CachedProperty.def(
		"wrap_descriptor",
		&CCachedProperty::wrap_descriptor,
		manage_new_object_policy(),
		"Wraps a descriptor as a cached property.",
		("descriptor", arg("owner")=object(), arg("name")=str())
	)
	.staticmethod("wrap_descriptor");

	scope().attr("cached_property") = scope().attr("CachedProperty");
}


//-----------------------------------------------------------------------------
// Exports CCachedGenerator.
//-----------------------------------------------------------------------------
void export_cached_generator(scope _cache)
{
	class_<CCachedGenerator, CCachedGenerator *> CachedGenerator("CachedGenerator",
		init<object>(
			(
				arg("generator")
			),
			"Represents a cached generator.\n"
			"If a :class:`core.cache.CachedProperty` returns a generator, it"
			" is being cached as an instance of this class. Then, when"
			" this instance is iterated over for the first time, the original"
			" generator is processed and the generated values are cached.\n"
			"\n"
			":param generator generator:\n"
			"	The wrapped generator instance.\n"
			"\n"
			":raises TypeError:\n"
			"	If the given generator is invalid.\n"
			":raises ValueError:\n"
			"	If the given generator is exhausted."
		)
	);

	CachedGenerator.def(
		"__iter__",
		&CCachedGenerator::__iter__,
		"Returns an iterator iterating over the generated values of the wrapped generator."
	);
}