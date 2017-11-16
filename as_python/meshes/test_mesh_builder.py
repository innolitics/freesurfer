import pytest

from meshes.mesh_builder import SimplePoint, SimplePointList, PointBuilder, Tetrahedron, IndexedPoint, Triangle


class TestSimplePoint:

    def test_constructor(self):
        actual = SimplePoint((1, 2, 3))
        assert actual.value == (1, 2, 3)
        assert len(actual) == 3
        assert actual[0] == 1
        assert actual[1] == 2
        assert actual[2] == 3

    def test_bad_constructor(self):
        with pytest.raises(ValueError) as error:
            uh_oh = SimplePoint((1, 2))

class TestSimplePointList:
    def test_construction(self):
        actual = SimplePointList([
            (1, 2, 3),
            (4, 5, 6),
            # (1, 2, 3),
            (7, 8, 9),
            (8, 9, 10),
        ])
        assert len(actual) == 4
        assert actual[2][0] == 7
        assert actual[3][2] == 10

class TestPointBuilder:
    def test_add_points(self):
        point_builder = PointBuilder()
        assert len(point_builder) == 0

        # add first point
        index = point_builder.add_point((1, 2, 3))
        assert index == 0
        assert len(point_builder) == 1
        assert point_builder[index][0] == 1
        assert point_builder[index][1] == 2
        assert point_builder[index][2] == 3

        # repeat first point
        index = point_builder.add_point((1, 2, 3))
        assert index == 0
        assert len(point_builder) == 1

        # differs in first coordinate
        index = point_builder.add_point((1.1, 2, 3))
        assert index == 1
        assert len(point_builder) == 2
        assert point_builder[index][0] == 1.1
        assert point_builder[index][1] == 2
        assert point_builder[index][2] == 3

        # differs in second coordinate
        index = point_builder.add_point((1, 2.2, 3))
        assert index == 2
        assert len(point_builder) == 3
        assert point_builder[index][0] == 1
        assert point_builder[index][1] == 2.2
        assert point_builder[index][2] == 3

        # differs in third coordinate
        index = point_builder.add_point((1, 2, 3.3))
        assert index == 3
        assert len(point_builder) == 4
        assert point_builder[index][0] == 1
        assert point_builder[index][1] == 2
        assert point_builder[index][2] == 3.3

        # repeat first point
        index = point_builder.add_point((1, 2, 3))
        assert index == 0
        assert len(point_builder) == 4

    def test_indexed_point_list(self):
        point_builder = PointBuilder()
        expected_index = 0
        for z in [-2, 2]:
            for y in [-3, 3]:
                for x in [-4, 4]:
                    actual_index = point_builder.add_point([x, y, z])
                    assert expected_index == actual_index
                    expected_index += 1
        point_list = point_builder.extract_point_list()
        assert len(point_list) == 8
        index = 0
        for z in [-2, 2]:
            for y in [-3, 3]:
                for x in [-4, 4]:
                    point = point_list[index]
                    assert len(point) == 3
                    assert point.index == index
                    assert point[0] == x
                    assert point[1] == y
                    assert point[2] == z
                    index += 1

class TestTetrahedron:

    def setup(self):
        point_builder = PointBuilder()
        for z in [-2, 2]:
            for y in [-3, 3]:
                for x in [-4, 4]:
                    point_builder.add_point([x, y, z])
        self.points = point_builder.extract_point_list()

    def test_triangle_not_enough_points(self):
        with pytest.raises(ValueError) as error:
            uh_oh = Triangle([self.points[0], self.points[1]])

    def test_triangle_too_many_points(self):
        with pytest.raises(ValueError) as error:
            uh_oh = Triangle([self.points[0], self.points[1], self.points[2], self.points[3]])

    def test_tetrahedron_not_enough_points(self):
        with pytest.raises(ValueError) as error:
            uh_oh = Tetrahedron([self.points[0], self.points[1], self.points[2]])

    def test_tetrahedron_too_many_points(self):
        with pytest.raises(ValueError) as error:
            uh_oh = Tetrahedron([self.points[0], self.points[1], self.points[2], self.points[3], self.points[4]])

    def test_tetrahedron_construction(self):
        build_points = [self.points[7], self.points[1], self.points[2], self.points[4]]
        tetrahedron = Tetrahedron(build_points)
        actual_points = tetrahedron.points
        assert len(actual_points) == 4
        assert actual_points[0].index == 7
        assert actual_points[1].index == 1
        assert actual_points[2].index == 2
        assert actual_points[3].index == 4
        actual_triangles = tetrahedron.triangles
        assert len(actual_triangles) == 4
