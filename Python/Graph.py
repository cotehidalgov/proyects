__author__ = 'MariaJose'
# -*- encoding: utf-8-*-
import sistema
from List import List
from BinaryTree import BinaryTree
from Formas import Par, Triangulo, Cuadrado


def escribir(linea, nombre):
    archivo = open("{}.txt".format(nombre), "a")
    archivo.write(linea + "\n")
    archivo.close()


class Graph:
    def __init__(self):
        self.port_tree = BinaryTree()
        self.connections = List()
        self.lista_triangulos = List()
        self.lista_cuadrados = List()
        self.lista_pisados = List()

    def recorrer(self):

        puerto_inicio = self.port_tree.add(sistema.preguntar_puerto_actual()[0],
                                           sistema.posibles_conexiones(), sistema.get_capacidad())
        contador = 0
        n = 0
        m = 0
        while True:

            camino = puerto_inicio.revisar()
            sistema.hacer_conexion(camino)
            puerto = self.port_tree.add(sistema.preguntar_puerto_actual()[0],
                                        sistema.posibles_conexiones(), sistema.get_capacidad())
            conexion = puerto_inicio.add_connection(camino, puerto_inicio, puerto)
            if puerto not in puerto_inicio.puertos_destino:
                puerto_inicio.puertos_destino.add(puerto)
                n += 1

            if conexion is not None:
                self.connections.add(conexion)
                contador = 0
                m += 1
            else:
                contador += 1
            puerto_inicio = puerto
            if contador == 1000:
                break
        print("Se ha recorrido todo el grafo, el cual tiene {} conexiones".format(m))

    def chequear_puertos(self):
        contador_conexiones = 0
        contador_puertos = 0
        for i in self.port_tree:
            contador_puertos += 1
            if i.posibles_conexiones == len(i.lista_conexiones):
                contador_conexiones += 1
        if contador_conexiones == contador_puertos:
            return False
        else:
            return True

    def anotar(self):
        for i in self.port_tree:
            escribir("PUERTO ID{}".format(i.ide), "red")
        for i in self.connections:
            escribir("CONEXION ID{} ID{}".format(i.origen.ide, i.destino.ide), "red")
        print("Se ha creado el archivo red.txt :)")

    def dijkstra(self):
        contador_puertos = 0
        for i in self.port_tree:
            contador_puertos += 1
        puerto_inicio = self.port_tree.nodo_raiz
        puerto_inicio.distancia = 0
        puerto_inicio.pisado = True
        puerto_inicio.origen = 0

        self.lista_pisados.add(puerto_inicio)

        def minimo(nodo):
            if not nodo.pisado:
                return nodo.distancia
            else:
                return float("inf")

        while len(self.lista_pisados) < contador_puertos:  # contador puertos esun numero fijo
            for i in puerto_inicio.puertos_destino:

                if i.distancia > puerto_inicio.distancia + 1:
                    i.distancia = puerto_inicio.distancia + 1
                    i.origen = puerto_inicio

            puerto_minimo = min(self.port_tree, key=minimo)
            puerto_minimo.pisado = True
            self.lista_pisados.add(puerto_minimo)
            puerto_inicio = puerto_minimo
        lista_recursiva = List()
        for puerto in self.lista_pisados:
            if sistema.puerto_final() == puerto.ide:  # estoy en bummer
                while puerto.ide != sistema.puerto_inicio():
                    lista_recursiva.add(puerto)
                    puerto = puerto.origen
                break
        escribir("El ide de Bummer es {}".format(sistema.puerto_final()), "rutaABummer")
        for i in lista_recursiva:
            escribir("Para el nodo {} tenemos que su origen es {}".format(i.ide, i.origen.ide), "rutaABummer")
        print("Se ha creado el archivo rutaABummer.txt :)")

    def doble_sentido(self):
        lista_pares = List()
        for puerto1 in self.port_tree:
            for puerto2 in self.port_tree:
                if puerto2 in puerto1.puertos_destino:  # significa que el puerto 1 tiene como puerto destino al piuerto 2
                    if puerto1 in puerto2.puertos_destino:
                        par = Par(puerto1, puerto2)
                        if par.chequear(lista_pares):
                            lista_pares.add(par)

        for par in lista_pares:
            escribir("PAR {} {} ".format(par.derecha.ide, par.izquierda.ide), "rutasDobleSentido")
        for par in lista_pares:
            ruta = par.calcular_ruta(lista_pares)
            escribir(ruta, "rutasDobleSentido")
        print("Se ha creado el archivo rutasDobleSentido.txt :)")

    def ciclos(self):

        for puerto1 in self.port_tree:
            for puerto2 in self.port_tree:
                if puerto1 in puerto2.puertos_destino:
                    for puerto3 in puerto1.puertos_destino:
                        if puerto2 in puerto3.puertos_destino:
                            triangulo = Triangulo(puerto1, puerto3, puerto2)
                            if triangulo.chequear(self.lista_triangulos):
                                self.lista_triangulos.add(triangulo)
                        for puerto4 in puerto3.puertos_destino:
                            if puerto2 in puerto4.puertos_destino:
                                cuadrado = Cuadrado(puerto1, puerto3, puerto4, puerto2)
                                if cuadrado.chequear(self.lista_cuadrados):
                                    self.lista_cuadrados.add(cuadrado)
        for i in self.lista_triangulos:
            escribir("{} {} {}".format(i.uno.ide, i.tres.ide, i.dos.ide), "ciclos")
        for i in self.lista_cuadrados:
            escribir("{} {} {} {}".format(i.uno.ide, i.tres.ide, i.cuatro.ide, i.dos.ide), "ciclos")

        print("Se ha creado el archivo ciclos.txt :)")

    def capacidad(self):
        ruta = List()
        puerto_inicio = self.port_tree.root
        ruta.add(puerto_inicio)
        while puerto_inicio.ide != sistema.puerto_final():
            for puerto in puerto_inicio.puertos_destino:
                ruta.add(puerto)

    def hackeo(self):
        lista_recursiva = List()
        for puerto in self.port_tree:
            escribir("PUERTO {}".format(puerto.ide), "noCycle")
        for puerto in self.lista_pisados:
            while puerto.ide != sistema.puerto_inicio():
                lista_recursiva.add(puerto)
                puerto = puerto.origen
            for i in lista_recursiva:
                escribir("CONEXION {} {}".format(i.origen.ide, i.ide), "noCycle")

        print("Se ha creado el archivo noCycle.txt :)")