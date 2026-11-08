Imports System

Module Player
    Sub Main()
        Dim height As Integer = Integer.Parse(Console.ReadLine())
        Dim width As Integer = Integer.Parse(Console.ReadLine())
        Dim count As Integer = Integer.Parse(Console.ReadLine())
        Dim dx() As Integer = {1, 0, -1, 0}
        Dim dy() As Integer = {0, 1, 0, -1}
        Dim actions() As String = {"A", "D", "E", "C"}
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
            Dim action As String = "B"
            For d As Integer = 0 To 3
                If cells(d) <> "_" Then Continue For
                Dim nx As Integer = xs(selfIndex) + dx(d)
                Dim ny As Integer = ys(selfIndex) + dy(d)
                Dim safe As Boolean = True
                For i As Integer = 0 To selfIndex - 1
                    If Math.Abs(nx - xs(i)) + Math.Abs(ny - ys(i)) <= 1 Then
                        safe = False
                        Exit For
                    End If
                Next
                If safe Then
                    action = actions(d)
                    Exit For
                End If
            Next
            Console.WriteLine(action)
            Console.Out.Flush()
        End While
    End Sub
End Module
