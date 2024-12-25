class Environment:
    def __init__(self, parent=None):
        self.parent = parent
        self.variables = {}

    def define(self, name, value):
        """
        Defines a new variable or updates an existing variable in the current environment.
        """
        self.variables[name] = value

    def lookup(self, name):
        """
        Looks up a variable in the current environment or recursively in parent environments.
        """
        if name in self.variables:
            return self.variables[name]
        elif self.parent:
            return self.parent.lookup(name)
        else:
            raise NameError(f"Variable '{name}' is not defined.")

    def update(self, name, value):
        """
        Updates a variable's value. If it doesn't exist in the current environment,
        checks parent environments.
        """
        if name in self.variables:
            self.variables[name] = value
        elif self.parent:
            self.parent.update(name, value)
        else:
            raise NameError(f"Variable '{name}' is not defined.")
