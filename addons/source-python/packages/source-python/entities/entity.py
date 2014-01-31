# ../entities/entity.py

# =============================================================================
# >> IMPORTS
# =============================================================================
# Source.Python Imports
#from Source import Binutils
from conversions_c import edict_from_index
from public import public
#   Entities
#from entities.functions import EntityFunctions
from entities.keyvalues import EntityKeyValues
from entities.offsets import EntityOffsets
from entities.properties import EntityProperties
#from entities.specials import _EntitySpecials


# =============================================================================
# >> CLASSES
# =============================================================================
#class BaseEntity(_EntitySpecials):
@public
class BaseEntity(object):
    '''Class used to interact directly with entities'''

    def __new__(cls, index, *entities):
        '''Override the __new__ class method to verify the given index
            is of the correct entity type and add the index attribute'''

        # Get the given indexes edict
        edict = edict_from_index(index)

        # Is the edict valid?
        if not edict or edict.is_free():

            # If not raise an error
            raise ValueError(
                'Index "{0}" is not a proper entity index'.format(index))

        # Create the object
        self = object.__new__(cls)

        # Set the entity's base attributes
        self._index = index
        self._edict = edict
        self._entities = frozenset(list(entities) + ['entity'])

        # Return the instance
        return self

    def __getattr__(self, attr):
        '''Finds if the attribute is valid and returns the appropriate value'''

        # Loop through all instances (used to get edict/IPlayerInfo attributes)
        for instance in self.instances:

            # Does the current instance contain the given attribute?
            if hasattr(instance, attr):

                # Return the instance's value for the given attribute
                return getattr(instance, attr)

        # Is the attribute a property of this entity?
        if attr in self.properties:

            # Return the property's value
            return self._get_property(attr)

        # Is the attribute a keyvalue of this entity?
        if attr in self.keyvalues:

            # Return the keyvalue's value
            return self._get_keyvalue(attr)

        # Is the attribute an offset of this entity?
        if attr in self.offsets:

            # Return the offset's value
            return self._get_offset(attr)

        '''

        # Is the attribute a function of this entity?
        if attr in self.functions:

            # Return the function
            return self._get_function(attr)

        '''

        # If the attribute is not found, raise an error
        raise AttributeError('Attribute "{0}" not found'.format(attr))

    def _get_property(self, item):
        '''Gets the value of the given property'''

        # Get the property's type
        prop_type = self.properties[item].type

        # Get the property's value
        value = getattr(self.edict, 'get_prop_{0}'.format(
            prop_type))(self.properties[item].prop)

        # Is the property a True/False property?
        if 'True' in self.properties[item]:

            # Return if the current value equals the "True" value
            return value == self.properties[item]['True']

        # Return the value of the property
        return value

    def _get_keyvalue(self, item):
        '''Gets the value of the given keyvalue'''

        # Get the keyvalue's type
        kv_type = self.keyvalues[item].type

        # Return the value of the given keyvalue
        return getattr(self.edict, 'get_keyvalue_{0}'.format(kv_type))(item)

    def _get_offset(self, item):
        '''Gets the value of the given offset'''

        # Get the offset's type
        offset_type = self.offsets[item].type

        # Get the CPointer instance for the entity
        pointer = CPointer(self.pointer)

        # Return the value of the offset
        return getattr(pointer, 'get_{0}'.format(
            offset_type))(self.offsets[item].offset)

    def _get_function(self, item):
        '''Calls a dynamic function'''

        # Does the entity's pointer need to be added to the arguments?
        if self.functions[item].pointer_index != -1:

            # Set the entity's pointer as the current one
            self.functions[item].current_pointer = self.pointer

        # Return the pre call function method
        return self.functions[item]._pre_call_function

    def __setattr__(self, attr, value):
        '''Finds if the attribute is value and sets its value'''

        # Is the given attribute private?
        if attr.startswith('_'):

            # Get the name of the private attribute
            name = attr[1:]

            # Is the attribute a property?
            if (name in dir(self) and isinstance(
                    getattr(self.__class__, name), property)):

                # Set the private attribute's value
                super(BaseEntity, self).__setattr__(attr, value)

                # No need to go further
                return

            # If not a property, do not allow the private attribute
            raise ValueError(
                'Invalid private attribute "{0}" given.'.format(attr))

        # Is the attribute a property of this entity?
        if attr in self.properties:

            # Set the property's value
            self._set_property(attr, value)

        # Is the attribute a keyvalue of this entity?
        elif attr in self.keyvalues:

            # Set the keyvalue's value
            self._set_keyvalue(attr, value)

        # Is the attribute an offset of this entity?
        elif attr in self.offsets:

            # Set the offset's value
            self._set_offset(attr, value)

        # Was the attribute not found?
        else:

            # If the attribute is not found, just set the attribute
            super(BaseEntity, self).__setattr__(attr, value)

    def _set_property(self, item, value):
        '''Sets the value of the given propery'''

        # Get the property's type
        prop_type = self.properties[item].type

        # Is the property a True/False property?
        if 'True' in self.properties[item]:

            # Get the exact value to set the property to
            value = self.properties[item][str(value)]

        # Set the property's value
        getattr(prop, 'set_prop_{0}'.format(
            prop_type))(self.properties[item].prop, value)

    def _set_keyvalue(self, item, value):
        '''Sets the value of the given keyvalue'''

        # Get the keyvalue's type
        kv_type = self.keyvalues[item]

        # Set the keyvalue's value
        getattr(self.edict, 'set_keyvalue_{0}'.format(kv_type))(item, value)

    def _set_offset(self, item, value):
        '''Sets the value of the given offset'''

        # Get the offset's type
        offset_type = self.offsets[item].type

        # Get the CPointer instance for the entity
        pointer = CPointer(self.pointer)

        # Set the offset's value
        getattr(pointer, 'set_{0}'.format(
            offset_type))(self.offsets[item].offset, value)

    def get_color(self):
        '''Returns a 4 part tuple (RGBA) for the entity's color'''

        # Get the render value
        value = self.render

        # Return a tuple with the RGBA values
        return (
            value & 0xff, (value & 0xff00) >> 8,
            (value & 0xff0000) >> 16, (value & 0xff000000) >> 24)

    def set_color(self, args):
        '''Sets the entity's color to the given RGBA values'''

        # Are the the correct number of arguments?
        if not len(args) in (3, 4):

            # Raise an error
            raise TypeError(
                'set_color() requires 3 or 4 ' +
                'arguments, {0} were given'.format(len(args)))

        # Get the RGB values
        red, green, blue = args[:3]

        # Get the value using the RGB values
        value = red + (green << 8) + (blue << 16)

        # Was an alpha value passed?
        if len(args) == 3:

            # Get the current alpha value
            alpha = (self.render & 0xff000000)

        # Otherwise
        else:

            # Get the 4th value passed
            alpha = args[3]

        # Add the alpha value to the RGB value
        value += alpha << 24

        # Set the rendermode
        self.rendermode = self.rendermode | 1

        # Set the renderfx
        self.renderfx = self.renderfx | 256

        # Set the entity's color
        self.render = value

    # Set the "color" property for BaseEntity
    color = property(get_color, set_color)

    @property
    def index(self):
        '''Returns the entity's index'''
        return self._index

    @property
    def edict(self):
        '''Returns the entity's edict instance'''
        return self._edict

    @property
    def entities(self):
        '''Returns the set of entity names to use for the instance'''
        return self._entities

    @property
    def instances(self):
        '''Yields the entity's edict instance'''
        yield self.edict

    @property
    def basehandle(self):
        '''Returns the entity's BaseEntityHandle instance'''
        return self.edict.get_networkable(
            ).get_entity_handle().get_ref_ehandle()

    @property
    def inthandle(self):
        '''Returns the entity's integer handle'''
        return self.basehandle.to_int()

    @property
    def classname(self):
        '''Returns the classname of the entity'''
        return self.edict.get_class_name()

    @property
    def pointer(self):
        '''Returns the entity's pointer'''
        return self.edict.get_unknown().get_base_entity()

    @property
    def properties(self):
        '''Returns all properties for all entities'''
        return EntityProperties.get_game_attributes(self.entities)

    @property
    def keyvalues(self):
        '''Returns all keyvalues for all entities'''
        return EntityKeyValues.get_game_attributes(self.entities)

    @property
    def offsets(self):
        '''Returns all offsets for all entities'''
        return EntityOffsets.get_game_attributes(self.entities)

    @property
    def functions(self):
        '''Returns all dynamic calling functions for all entities'''
        return EntityFunctions.get_game_attributes(self.entities)