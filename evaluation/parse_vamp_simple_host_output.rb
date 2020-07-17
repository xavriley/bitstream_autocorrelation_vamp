csv = IO.read "./example_pitch_tracking.csv"

output = csv.each_line.map {|l| l.match(/(\d+\.\d+): (\d+\.?(\d+)?)/).values_at(1,2).map(&:to_f).map {|f| f.round(6) } }

puts output.map {|l| l.join(",") }.join("\n")
