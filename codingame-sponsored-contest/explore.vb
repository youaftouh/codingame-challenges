Imports System
Imports System.Collections.Generic

Module Player
    Sub Main()
        Dim height As Integer = Integer.Parse(Console.ReadLine())
        Dim width As Integer = Integer.Parse(Console.ReadLine())
        Dim count As Integer = Integer.Parse(Console.ReadLine())
        Dim dx() As Integer = {1, 0, -1, 0}
        Dim dy() As Integer = {0, 1, 0, -1}
        Dim actions() As String = {"A", "D", "E", "C"}
        Dim terrain(width * height - 1) As Integer
        Dim visits(width * height - 1) As Integer
        While True
            Dim north As String = Console.ReadLine()
            If north Is Nothing Then Exit Sub
            Dim east As String = Console.ReadLine()
            Dim south As String = Console.ReadLine()
            Dim west As String = Console.ReadLine()
            Dim cells() As String = {east, south, west, north}
            Dim xs(count - 1), ys(count - 1) As Integer
            For i As Integer = 0 To count - 1
                Dim parts() As String = Console.ReadLine().Split(New Char() {" "c}, StringSplitOptions.RemoveEmptyEntries)
                xs(i) = Integer.Parse(parts(0))
                ys(i) = Integer.Parse(parts(1))
            Next
            Dim selfIndex As Integer = count - 1
            Dim px As Integer = xs(selfIndex), py As Integer = ys(selfIndex)
            Dim start As Integer = py * width + px
            terrain(start) = 1
            visits(start) += 1
            For d As Integer = 0 To 3
                Dim nx As Integer = (px + dx(d) + width) Mod width, ny As Integer = (py + dy(d) + height) Mod height
                If nx >= 0 AndAlso nx < width AndAlso ny >= 0 AndAlso ny < height Then
                    terrain(ny * width + nx) = If(cells(d) = "#", -1, 1)
                End If
            Next
            Dim action As String = "B"
            Dim best As Double = Double.NegativeInfinity
            For d As Integer = 0 To 3
                If cells(d) <> "_" Then Continue For
                Dim nx As Integer = (xs(selfIndex) + dx(d) + width) Mod width
                Dim ny As Integer = (ys(selfIndex) + dy(d) + height) Mod height
                Dim safe As Boolean = True
                For i As Integer = 0 To selfIndex - 1
                    If Math.Abs(nx - xs(i)) + Math.Abs(ny - ys(i)) <= 1 Then
                        safe = False
                        Exit For
                    End If
                Next
                If safe Then
                    Dim first As Integer = ny * width + nx
                    Dim distance(width * height - 1) As Integer
                    Array.Fill(distance, -1)
                    Dim queue As New Queue(Of Integer)()
                    queue.Enqueue(first)
                    distance(first) = 0
                    distance(start) = 0
                    Dim value As Double = -10000
                    While queue.Count > 0
                        Dim cell As Integer = queue.Dequeue()
                        Dim cx As Integer = cell Mod width, cy As Integer = cell \ width
                        Dim steps As Integer = distance(cell)
                        Dim nearest As Integer = 999
                        For e As Integer = 0 To selfIndex - 1
                            nearest = Math.Min(nearest, Math.Abs(cx - xs(e)) + Math.Abs(cy - ys(e)))
                        Next
                        If visits(cell) = 0 Then
                            Dim reward As Double = 100.0 - steps * 3.0 + Math.Min(nearest, 6) * 0.5
                            value = Math.Max(value, reward)
                        End If
                        For k As Integer = 0 To 3
                            Dim ax As Integer = (cx + dx(k) + width) Mod width, ay As Integer = (cy + dy(k) + height) Mod height
                            If ax < 0 OrElse ax >= width OrElse ay < 0 OrElse ay >= height Then Continue For
                            Dim adjacent As Integer = ay * width + ax
                            If terrain(adjacent) <> 1 OrElse distance(adjacent) >= 0 Then Continue For
                            Dim blocked As Boolean = False
                            For e As Integer = 0 To selfIndex - 1
                                If Math.Abs(ax - xs(e)) + Math.Abs(ay - ys(e)) <= 1 Then blocked = True
                            Next
                            If blocked Then Continue For
                            distance(adjacent) = steps + 1
                            queue.Enqueue(adjacent)
                        Next
                    End While
                    value -= visits(first) * 0.1
                    If value > best Then
                        best = value
                        action = actions(d)
                    End If
                End If
            Next
            Console.WriteLine(action)
            Console.Out.Flush()
        End While
    End Sub
End Module
