STDOUT.sync = true # DO NOT REMOVE
# Auto-generated code below aims at helping you parse
# the standard input according to the problem statement.


# game loop
loop do
  resources = gets.to_i
  num_travel_routes = gets.to_i
  num_travel_routes.times do
    building_id_1, building_id_2, capacity = gets.split.map { |x| x.to_i }
  end
  num_pods = gets.to_i
  num_pods.times do
    pod_properties = gets.chomp
  end
  num_new_buildings = gets.to_i
  num_new_buildings.times do
    building_properties = gets.chomp
  end

  # Write an action using puts
  # To debug: STDERR.puts "Debug messages..."

  puts "TUBE 0 1;TUBE 0 2;POD 42 0 1 0 2 0 1 0 2" # TUBE | UPGRADE | TELEPORT | POD | DESTROY | WAIT
end
