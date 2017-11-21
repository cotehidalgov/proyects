__author__ = 'MariaJose'


class Node:
    def __init__(self, ide=None):
        self.next = None
        self.ide = ide


class List:
    def __init__(self):
        self.tail = None
        self.head = None

    def add(self, ide):
        if not self.head:
            self.head = Node(ide)
            self.tail = self.head
        else:
            self.tail.next = Node(ide)
            self.tail = self.tail.next

    def delete(self, node_id):
        actual_node = self.head
        while actual_node:
            if actual_node.next.ide == node_id:
                actual_node.next = actual_node.next.next
                break
            else:
                actual_node = actual_node.next

    def __repr__(self):
        rep = "["
        actual_node = self.head
        while actual_node:
            if actual_node == self.tail:
                rep += "{}".format(actual_node.ide)
            else:
                rep += "{}, ".format(actual_node.ide)
            actual_node = actual_node.next
        return rep + "]"

    def __iter__(self):
        actual_node = self.head
        while actual_node:
            yield actual_node.ide
            actual_node = actual_node.next

    def __len__(self):
        actual_node = self.head
        len = 0
        while actual_node:
            len += 1
            actual_node = actual_node.next
        return len
