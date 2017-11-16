class SimpleMesh:
    def __init__(self):
        self.tetrahedrons = []
        self.vertices = []
        pass

    @property
    def number_of_tetrahedrons(self):
        return len(self.tetrahedrons)

    @property
    def number_of_vertices(self):
        return 4 * self.number_of_tetrahedrons


class SimplePoint:
    def __init__(self, coordinates):
        self.value = tuple(coordinates)
        if(len(coordinates) != 3):
            raise ValueError("point coordiates must be length of 3")

    def __len__(self):
        return 3

    def __getitem__(self, item):
        return self.value[item]


class SimplePointList:
    def __init__(self, points):
        self.points = [SimplePoint(point) for point in points]

    def __len__(self):
        return len(self.points)

    def __getitem__(self, item):
        return self.points[item]

class IndexedPoint:
    def __init__(self, points, index):
        self.points = points
        self.index = index

    def __len__(self):
        return len(self.points[self.index])

    def __getitem__(self, item):
        return self.points[self.index][item]

class IndexedPointList:
    def __init__(self, points):
        self.points = points
        self.indexed_points = [IndexedPoint(self.points, index) for index, point in enumerate(self.points)]

    def __len__(self):
        return len(self.points)

    def __getitem__(self, item):
        return self.indexed_points[item]


def simple_mesh_builder(tetrahedrons):
    simple_mesh = SimpleMesh()
    simple_mesh.tetrahedrons = tetrahedrons
    return simple_mesh

class PointBuilder:
    def __init__(self):
        self.have_list = {}
        self.points = []
        self._point_count = 0

    def __len__(self):
        return self._point_count

    def __getitem__(self, item):
        return self.points[item]

    def add_point(self, point):
        point_index = self.have_list\
            .setdefault(point[0], {})\
            .setdefault(point[1], {})\
            .setdefault(point[2], self._point_count)
        if(point_index == self._point_count):
            self.points.append(point)
            self._point_count += 1
        return point_index

    def extract_point_list(self):
        return IndexedPointList(self.points)

class Triangle:
    def __init__(self, points):
        if(len(points) != 3):
            raise ValueError("triangle must have exactly 3 points")
        pass

class Tetrahedron:
    def __init__(self, points):
        if(len(points) != 4):
            raise ValueError("terahedron must have exactly 4 points")
        self.points = points
        self.triangles = [
            Triangle([self.points[3], self.points[2], self.points[1]]),
            Triangle([self.points[2], self.points[3], self.points[0]]),
            Triangle([self.points[1], self.points[0], self.points[3]]),
            Triangle([self.points[0], self.points[1], self.points[2]]),
        ]


class TetrahedronBuilder:
    def __init__(self):
        pass