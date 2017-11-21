__author__ = "MariaJose"

from List import List


class Connection:
    def __init__(self, camino, origen, destino):
        self.origen = origen
        self.camino = camino
        self.destino = destino
        self.tipo = "Normal"
        self.visitas = 1
        self.capacidad_maxima = min(self.origen.capacidad, self.destino.capacidad)


class Port:
    def __init__(self, ide, possible_connections, capacidad, padre=None):
        self.ide = ide
        self.possible_connections = possible_connections
        self.lista_conexiones = List()
        self.puertos_destino = List()

        self.father = padre
        self.left = None
        self.right = None

        self.distancia = float("inf")
        self.pisado = False
        self.origen = -1
        self.capacidad = capacidad

    def add_connection(self, camino, origen, destino):
        for connection in self.lista_conexiones:
            if connection.camino == camino:  # No esta entonces la creo
                connection.visitas += 1
                return
        conexion = Connection(camino, origen, destino)
        self.lista_conexiones.add(conexion)
        return conexion

    def revisar(self):
        if len(self.lista_conexiones) == self.possible_connections:
            return min(self.lista_conexiones, key=lambda i: i.visitas).camino
        else:
            return len(self.lista_conexiones)

    def __iter__(self):
        yield self
        if self.left is not None:
            for i in iter(self.left):
                yield i
        if self.right is not None:
            for i in iter(self.right):
                yield i


class BinaryTree:
    def __init__(self, root=None):
        self.root = root

    def add(self, ide, possible_connections, capacity):
        actual_node = self.root
        if not actual_node:
            self.root = Port(ide, possible_connections, capacity)
            return self.root
        else:
            while True:
                if actual_node.ide > ide:
                    if actual_node.left is not None:
                        actual_node = actual_node.left
                    else:
                        actual_node.left = Port(ide, possible_connections, capacity, actual_node.ide)
                        return actual_node.left
                elif actual_node.ide < ide:
                    if actual_node.right is not None:
                        actual_node = actual_node.right
                    else:
                        actual_node.right = Port(ide, possible_connections, capacity, actual_node.ide)
                        return actual_node.right
                else:
                    return actual_node

    def __iter__(self):
        actual_node = self.root
        return iter(actual_node)
