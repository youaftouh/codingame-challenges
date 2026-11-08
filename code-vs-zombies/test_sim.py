import unittest
import bot


class SimulatorTests(unittest.TestCase):
    def test_diagonal_rounding(self):
        self.assertEqual(bot.move(0, 0, 500, 500, 400), (282, 282))

    def test_shooting_precedes_eating(self):
        ash, humans, zombies, points = bot.advance(
            (3000, 0), [(5000, 0)], [(5000, 0)], (4000, 0))
        self.assertEqual((humans, zombies, points), ([(5000, 0)], [], 10))

    def test_combo_uses_human_count_squared(self):
        _, humans, zombies, points = bot.advance(
            (4000, 4000), [(0, 0), (15999, 8999)],
            [(4000, 5000), (5000, 4000)], (4000, 4000))
        self.assertEqual(len(humans), 2)
        self.assertEqual(zombies, [])
        self.assertEqual(points, 2**2 * 10 * (1+2))

    def test_zombie_targets_ash_before_ash_moves(self):
        ash, _, zombies, _ = bot.advance(
            (0, 0), [(10000, 0)], [(4000, 0)], (0, 1000))
        self.assertEqual(ash, (0, 1000))
        self.assertEqual(zombies, [(3600, 0)])


if __name__ == '__main__':
    unittest.main()
