import pytest
from copy import deepcopy

from meshes.mesh_builder import simple_mesh_builder


def test_that_testing_happens():
    x = 7
    y = 9
    assert x != y


BUILDER_NAMES = ['simple']

BIG_TETRAHEDRA_POINTS = [
            (1, 1, 1),
            (1, -1, -1),
            (-1, 1, -1),
            (-1, -1, 1)
        ]

CORNER_TETRAHEDRAS = [
    [
        (-1, -1, -1),
        (1, -1, -1),
        (-1, 1, -1),
        (-1, -1, 1),
    ],
    [
        (1, 1, 1),
        (-1, 1, 1),
        (-1, 1, -1),
        (-1, -1, 1)
    ],
    [
        (1, 1, 1),
        (1, -1, -1),
        (1, -1, 1),
        (-1, -1, 1)
    ],
    [
        (1, 1, 1),
        (1, -1, -1),
        (-1, 1, -1),
        (1, 1, -1)
    ],
]

# class TestPointBuilders:
#     def setup(self):
#         self.builders = {
#             'simple': SimplePointBuilder(),
#         }

class TestMeshBuilders:
    def setup(self):
        self.big_tetra_points = deepcopy(BIG_TETRAHEDRA_POINTS)
        self.corner_tetras = deepcopy(CORNER_TETRAHEDRAS)
        self.cube_tetras = deepcopy([BIG_TETRAHEDRA_POINTS] + CORNER_TETRAHEDRAS)
        self.builders = {
            'simple': simple_mesh_builder,
        }

    def build_mesh(self, builder_name, tetrahedrons):
        return self.builders[builder_name](tetrahedrons)

    def build_single_tetrahedron(self, builder_name):
        return self.build_mesh(builder_name, [self.big_tetra_points])

    @pytest.mark.parametrize("builder_name", BUILDER_NAMES)
    def test_single_tetrahedron(self, builder_name):
        mesh = self.build_single_tetrahedron(builder_name)
        assert mesh.number_of_tetrahedrons == 1
        assert mesh.number_of_vertices == 4
