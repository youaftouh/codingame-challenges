using System;
using System.Linq;
using System.Collections.Generic;

class Node
{
    public int ID;
    public int X, Y;
    public List<Node> Neighbors = new List<Node>();
    public int MyUnits;
    public int OpponentUnits;
    public int[] Dist = new int[50];

    public Node(int id, int x, int y)
    {
        this.X = x;
        this.Y = y;
        this.ID = id;
    }

    public void Clear()
    {
        Neighbors.Clear();
        MyUnits = 0;
        OpponentUnits = 0;
        for (int i = 0; i < Dist.Length; i++) Dist[i] = 1000;
    }

    public void ComputeDist()
    {
        Queue<Node> queue = new Queue<Node>();
        queue.Enqueue(this);
        Dist[this.ID] = 0;
        while (queue.Count > 0)
        {
            Node current = queue.Dequeue();
            foreach (Node next in current.Neighbors)
            {
                if (Dist[next.ID] != 1000) continue;
                Dist[next.ID] = 1 + Dist[current.ID];
                queue.Enqueue(next);
            }
        }
    }

    public string Move(Node target)
    {
        if (this == target) return "";
        return "MOVE " + this.ID + " " + Neighbors.OrderBy(n => n.Dist[target.ID]).First().ID + " 1";
    }

    private static bool ccw(Node a, Node b, Node c)
    {
        return (c.Y - a.Y) * (b.X - a.X) > (b.Y - a.Y) * (c.X - a.X);
    }

    private static bool isCrossing(Node n1a, Node n1b, Node n2a, Node n2b)
    {
        HashSet<Node> nodes = new HashSet<Node>();
        nodes.Add(n1a);
        nodes.Add(n1b);
        nodes.Add(n2a);
        nodes.Add(n2b);
        if (nodes.Count < 4) return false;

        return ccw(n1a, n2a, n2b) != ccw(n1b, n2a, n2b) && ccw(n1a, n1b, n1a) != ccw(n1a, n1b, n2b);
    }

    public bool CanConnect(Node to, List<Node> graph)
    {
        if (this.PythDist(to) > 400) return false;
        foreach (Node n2a in graph)
        {
            foreach (Node n2b in n2a.Neighbors)
            {
                if (isCrossing(this, to, n2a, n2b)) return false;
            }
        }
        return true;
    }

    private double PythDist(Node to)
    {
        int dx = this.X - to.X;
        int dy = this.Y - to.Y;
        return Math.Sqrt(dx * dx + dy * dy);
    }

    public override string ToString()
    {
        return ID.ToString();
    }
}

class Triangle
{
    public Node Node1;
    public Node Node2;
    public Node Node3;
    public int Owner;
    public bool MeCanCapture;
    public bool OppCanCapture;

    public Triangle(Node node1, Node node2, Node node3, int owner, bool meCanCapture, bool oppCanCapture)
    {
        this.Node1 = node1;
        this.Node2 = node2;
        this.Node3 = node3;
        this.Owner = owner;
        this.MeCanCapture = meCanCapture;
        this.OppCanCapture = oppCanCapture;
    }

    public List<Node> Corners => new List<Node> { Node1, Node2, Node3 };
    public List<Node> Neighbors => Corners.SelectMany(c => c.Neighbors).ToList();

    public IEnumerable<List<Node>> CornerPermutations()
    {
        yield return new List<Node> { Node1, Node2, Node3 };
        yield return new List<Node> { Node1, Node3, Node2 };
        yield return new List<Node> { Node2, Node1, Node3 };
        yield return new List<Node> { Node2, Node3, Node1 };
        yield return new List<Node> { Node3, Node1, Node2 };
        yield return new List<Node> { Node3, Node2, Node1 };
    }

    public override string ToString()
    {
        return Node1.ID + " " + Node2.ID + " " + Node3.ID;
    }

    internal string Spawn(Node spawn)
    {
        return "SPAWN " + spawn + " " + string.Join(" ", Corners.Where(c => c != spawn));
    }
}

class Solution
{
    static string ReadLine()
    {
        string line = Console.ReadLine();
        //Console.Error.WriteLine(line);
        return line;
    }

    static void Main(string[] args)
    {
        string[] inputs;
        int nodeCount = int.Parse(ReadLine());
        List<Node> nodes = new List<Node>();
        for (int i = 0; i < nodeCount; i++)
        {
            inputs = ReadLine().Split(' ');
            int nodeId = int.Parse(inputs[0]);
            int nodeX = int.Parse(inputs[1]);
            int nodeY = int.Parse(inputs[2]);
            nodes.Add(new Node(nodeId, nodeX, nodeY));
        }

        // game loop
        while (true)
        {
            int[] scores = ReadLine().Split().Select(int.Parse).ToArray();
            nodes.ForEach(n => n.Clear());
            for (int i = 0; i < nodeCount; i++)
            {
                inputs = ReadLine().Split(' ');
                int[] nums = inputs.Select(int.Parse).ToArray();
                nodes[nums[0]].MyUnits = nums[1];
                nodes[nums[0]].OpponentUnits = nums[2];
            }
            int linkCount = int.Parse(ReadLine());
            for (int i = 0; i < linkCount; i++)
            {
                inputs = ReadLine().Split(' ');
                int node1 = int.Parse(inputs[0]);
                int node2 = int.Parse(inputs[1]);
                nodes[node1].Neighbors.Add(nodes[node2]);
                nodes[node2].Neighbors.Add(nodes[node1]);
            }
            int triangleCount = int.Parse(ReadLine());
            List<Triangle> triangles = new List<Triangle>();
            for (int i = 0; i < triangleCount; i++)
            {
                inputs = ReadLine().Split(' ');
                int node1 = int.Parse(inputs[0]);
                int node2 = int.Parse(inputs[1]);
                int node3 = int.Parse(inputs[2]);
                int owner = int.Parse(inputs[3]);
                bool meCanCapture = inputs[4] != "0";
                bool opponentCanCapture = inputs[5] != "0";
                triangles.Add(new Triangle(nodes[node1], nodes[node2], nodes[node3], owner, meCanCapture, opponentCanCapture));
            }
            int linkableCount = int.Parse(ReadLine());
            for (int i = 0; i < linkableCount; i++) ReadLine();

            nodes.ForEach(n => n.ComputeDist());

            List<Node> myUnitCells = new List<Node>();
            foreach (Node node in nodes)
            {
                for (int i = 0; i < node.MyUnits; i++) myUnitCells.Add(node);
            }

            string solution = "";
            if (myUnitCells.Count > 20)
            {
                List<Triangle> myTriangles = triangles.Where(t => t.Owner == 0 && t.Corners.All(c => c.MyUnits > 0)).ToList();
                if (myTriangles.Count > 0)
                {
                    Triangle attackSource = myTriangles.OrderByDescending(t => t.Neighbors.Max(n => n.OpponentUnits)).First();
                    Node attack = attackSource.Neighbors.OrderByDescending(n => n.OpponentUnits).First();
                    if (attack.OpponentUnits > 1)
                    {
                        solution = "ATTACK " + attackSource + " " + attack.ID + ";MSG ATTACK!;";
                    }
                    Node removeEdge = attackSource.Neighbors.Except(attackSource.Corners).OrderByDescending(n => n.OpponentUnits).First();
                    //if (attack.OpponentUnits > 0) solution = ";REMOVE_PATH " + attackSource + " " + removeEdge.ID + ";MSG ATTACK!";
                    //if (attackSource.ToString() == "14 23 38") solution = "ADD_PATH 38 14 23 5";
                    foreach (Node corner in attackSource.Corners)
                    {
                        if (solution != "") break;
                        foreach (Node partner in nodes.Where(n => n.Dist[corner.ID] == 2))
                        {
                            if (corner.CanConnect(partner, nodes))
                            {
                                List<Node> others = attackSource.Corners.ToList();
                                others.Remove(corner);
                                solution = $"ADD_PATH {corner} {others[0]} {others[1]} {partner};";
                                attackSource.Corners.ForEach(c => myUnitCells.Remove(c));
                                attackSource.Corners.ForEach(c => c.MyUnits--);
                                attackSource.Owner = -1;
                                break;
                            }
                        }
                    }
                }
            }

            int bestCost = 1000;
            List<Node> moving = new List<Node>();
            List<Node> moveFrom = new List<Node>();
            List<Node> moveTo = new List<Node>();
            if (myUnitCells.Count >= 3)
            {
                foreach (Triangle triangle in triangles.Where(t => t.Owner != 0 && t.MeCanCapture))
                {
                    foreach (List<Node> corners in triangle.CornerPermutations())
                    {
                        List<Node> sources = new List<Node>();
                        List<Node> free = myUnitCells.ToList();
                        int currentScore = 0;
                        foreach (Node target in corners)
                        {
                            int index = 0;
                            for (int i = 1; i < free.Count; i++)
                            {
                                if (target.Dist[free[i].ID] < target.Dist[free[index].ID]) index = i;
                            }
                            sources.Add(free[index]);
                            free.RemoveAt(index);
                            currentScore = Math.Max(currentScore, sources.Last().Dist[target.ID]);
                        }
                        if (currentScore < bestCost)
                        {
                            bestCost = currentScore;
                            moveFrom = sources.ToList();
                            moveTo = corners.ToList();
                            moving = sources;
                        }
                    }
                }
            }
            List<Node> targets = Mutate(triangles, myUnitCells, moveFrom, moveTo);
            solution += string.Join(";", Enumerable.Range(0, myUnitCells.Count).Select(i => myUnitCells[i].Move(targets[i])));

            int unitCount = myUnitCells.Count;
            foreach (Triangle triangle in triangles.Where(t => t.Owner == 0))
            {
                if (unitCount++ < nodes.Count)
                {
                    List<Node> corners = triangle.Corners.OrderBy(c => targets.Count(t => t == c)).ThenByDescending(c => c.Neighbors.Count).ToList();
                    double score = int.MinValue;
                    Node spawn = corners[0];
                    foreach (Node corner in corners)
                    {
                        targets.Add(corner);
                        double tmpScore = Score(triangles, targets);
                        targets.RemoveAt(targets.Count - 1);
                        if (tmpScore > score)
                        {
                            spawn = corner;
                            score = tmpScore;
                        }
                    }
                    solution += ";" + triangle.Spawn(spawn);
                    targets.Add(spawn);
                }
            }

            Console.Error.WriteLine("cost: " + bestCost);
            Console.WriteLine(solution);
        }
    }

    static Random random = new Random(0);
    static List<Node> Mutate(List<Triangle> triangles, List<Node> myUnits, List<Node> moveFrom, List<Node> moveTo)
    {
        List<Node> targets = new List<Node>();
        foreach (Node n in myUnits) targets.Add(null);
        for (int i = 0; i < moveFrom.Count; i++)
        {
            for (int j = 0; j < targets.Count; j++)
            {
                if (targets[j] == null && myUnits[j] == moveFrom[i])
                {
                    targets[j] = moveTo[i];
                    break;
                }
            }
        }

        for (int i = 0; i < targets.Count; i++)
        {
            if (targets[i] == null) targets[i] = myUnits[i].Neighbors[random.Next(myUnits[i].Neighbors.Count)];
        }
        double score = Score(triangles, targets);
        for (int mutation = 0; mutation < 10000; mutation++)
        {
            int index = random.Next(targets.Count);
            Node backup = targets[index];
            Node newNode = myUnits[index].Neighbors[random.Next(myUnits[index].Neighbors.Count)];
            targets[index] = newNode;
            double newScore = Score(triangles, targets);
            if (newScore >= score) score = newScore;
            else targets[index] = backup;
        }

        return targets;
    }

    static double Score(List<Triangle> triangles, List<Node> moveTo)
    {
        double result = 0;
        int[] unitCount = new int[50];
        foreach (Node to in moveTo) unitCount[to.ID]++;
        foreach (Triangle t in triangles)
        {
            if (t.MeCanCapture && unitCount[t.Node1.ID] > 0 && unitCount[t.Node2.ID] > 0 && unitCount[t.Node3.ID] > 0) result++;
            else if (t.Owner != 0 && !t.MeCanCapture && unitCount[t.Node1.ID] == 0 && unitCount[t.Node2.ID] == 0 && unitCount[t.Node3.ID] == 0) result += 0.5;
        }
        return result;
    }
}
