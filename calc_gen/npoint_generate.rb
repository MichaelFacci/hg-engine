require_relative 'helpers'
require 'json'
require_relative 'inc_converter'


dex_data = File.readlines('../armips/data/mondata.s')
move_data = File.readlines('../armips/data/moves.s')
set_data = File.readlines('../armips/data/trainers/trainers.s')
ls_data = File.readlines('../armips/data/levelupdata.s')
tm_data = File.readlines('../armips/data/tmlearnset.txt')
tutor_data = File.readlines('../armips/data/tutordata.txt')
enc_data = File.readlines('../armips/data/encounters.s')

poks = {}
pok_names = {}
ordered_poks = []

npoint_moves = {}
move_names = {}
ordered_moves = []
ordered_growths = []
sets = {}

encounters = {}

current_pok = nil
stat_names = ["hp", "at", "df", "sp", "sa", "sd"]
growths = {"GROWTH_MEDIUM_FAST" => 0, "GROWTH_ERRATIC" => 1, "GROWTH_FLUCTUATING" => 2, "GROWTH_MEDIUM_SLOW" => 3, "GROWTH_FAST" => 4, "GROWTH_SLOW" => 5}

forms = form_info



i = 0
while i < dex_data.length
	
	line = dex_data[i].strip

	# set current mon 
	if line[0..6] == "mondata"
		# Remove comments
  		line = line.split('//').first.strip


		pok_variable_name = line.split("\"")[0].split(" ")[-1][0..-2]
		pok_name = line.split("\"")[-1].gsub("♂", "-M").gsub("♀", "-F")

		#handle megas
		if pok_variable_name.include?("_MEGA_")
			pok_name = pok_variable_name.split("SPECIES_MEGA_")[-1].downcase.capitalize + "-Mega"
			pok_name = pok_name.gsub("_y-Mega", "-Mega-Y").gsub("_x-Mega", "-Mega-X") #handle charizard/mewtwo
		end

		#attempt to guess form names
		if pok_name == "-----" && pok_variable_name != ""
			pok_name = pok_variable_name.split("SPECIES_")[-1].downcase.capitalize.gsub("_galarian", "-Galar").gsub("_alolan", "-Alola").gsub("_paldean", "-Paldea")
		end
		
		current_pok = pok_name
		poks[pok_name] = {}
		pok_names[pok_variable_name] = pok_name
		ordered_poks << pok_name
	end

	i += 1
	# skip lines until next pokemon is found
	next if !current_pok 

	

	# set stats if unset
	if line[0..3] == "base" && !poks[current_pok]["bs"]
		stat_values = line[10..-1].split(",").map(&:strip).map(&:to_i)
		poks[current_pok]["bs"] = {}
		(0..5).each do |n|
			poks[current_pok]["bs"][stat_names[n]] = stat_values[n]
		end
	end



	if line.include?("growthrate")
		growth = growths[line.split(" ")[1].strip]
		ordered_growths << growth
	end

	#set typing if unset
	if line[0..3] == "type" && !poks[current_pok]["types"]
		types = line[5..-1].split(",").map(&:strip).map do |type|
			type[5..-1].downcase.capitalize
		end
		poks[current_pok]["types"] = types.uniq.map do |type|
			type.include?("type_fairy") ? "Fairy" : type
		end
		
	end

	if line[0..3] == "abil" && !poks[current_pok]["abs"]
		abilities = line.split(" ABILITY_")[1..2].map do |ab|
			showdown_parse(ab.gsub(",",""))
		end

		poks[current_pok]["abs"] = abilities
		current_pok = nil
	end
end


i = 0
current_move = nil
while i < move_data.length
	line = move_data[i].split('/*').first.strip

	#set current move
	if line[0..7] == "movedata"
		move_variable_name = line.split("\"")[0].split(" ")[-1][0..-2]
		move_name = line.split("\"")[-1]
		current_move = move_name
		npoint_moves[move_name] = {}
		move_names[move_variable_name] = move_name
		ordered_moves << move_name
	end

	i += 1
	next if !current_move

	# set category 
	if line[0..2] == "pss" 
		category = line[10..-1].downcase.capitalize
		npoint_moves[current_move]["category"] = category
	end

	# set basePower
	if line[0..3] == "base" 
		bp = line[10..-1].strip.to_i
		npoint_moves[current_move]["basePower"] = bp
	end

	# set type
	if line[0..3] == "type" 
		type = line[10..-1].downcase.capitalize
		npoint_moves[current_move]["type"] = type.split(" ")[0].gsub("Y_type_implemented)", "Fairy")
	end
end


i = 0
current_set = nil
current_tr_title = nil
current_battle_type = nil
current_tr_id = nil
current_sub_index = 0
set_constants = {}
trainer_counts = {}

rival_index = 0
male_rival = "Ethan"
female_rival = "Lyra"
passerby_rival = "Silver"

def safe_parse_move(line)
  return nil unless line&.include?("move ")
  move_str = line.strip.split("move ")[1]
  return nil if move_str.nil? || move_str.empty?
  parse_move(move_str)
end

## set data
while i < set_data.length
	line = set_data[i].strip

	if line.include?("equ")
		constant_name = line.split("equ")[0].strip
		begin
			constant_value = line.split("equ")[1].split(" ")[0].to_i
		rescue
			p "constant processing error: #{line}"
			constant_value = 0
		end
		set_constants[constant_name] = constant_value
	end


	if line[0..10] == "trainerdata" 
		current_tr_id = line.split(",")[0].split(" ")[-1].to_i
		tr_name = line.split("\"")[-1]


		# indicate starter choice for rival battles
		

		tr_class = set_data[i + 2].split("CLASS_")[-1].downcase.split("_").map(&:capitalize).join(" ").strip
		tr_title = "#{tr_class} #{tr_name}"

		if tr_title.include?(male_rival) || tr_title.include?(male_rival) || tr_title.include?(passerby_rival)
			tr_title += " |Starter #{rival_index % 3 + 1}|"
			rival_index += 1
		end

		# cleanup trainer classes with numbers in them
		tr_title = tr_title.gsub(/ \d+ /, ' ')

		trainer_counts[tr_title] ||= 0
		trainer_counts[tr_title] += 1

		
		if trainer_counts[tr_title] > 1
			tr_title += "#{trainer_counts[tr_title]} "
		end

		current_tr_title = tr_title
		current_battle_type = set_data[i + 9].include?("SINGLE") ? "Singles" : "Doubles"
		current_sub_index = 0
	end



	# relies on consistent formatting
	if line[0..4] == "level"
		level = parse_level set_constants, line


		# handle forms
		if set_data[i + 1].include?("monwithform")
			form_index = set_data[i + 1].split(",")[-1].strip.to_i
			species = pok_names[set_data[i + 1].strip.split(" ")[1].strip.gsub(",","")]
			if forms[species]
				species = "#{species}-#{forms[species][form_index - 1]}"
			end
			
		else
			species = pok_names[set_data[i + 1].strip.split(" ")[1].strip.gsub(",","")]
		end
			
		
		item = showdown_parse set_data[i + 2].gsub("item ", "").split("ITEM_")[-1].strip
		item = "" if item == "0"

		item = item.gsub("Never Melt", "Never-Melt").gsub("Heavy Duty", "Heavy-Duty")



		# handle megas
		if item[-3..-1] == "ite" && item != "Eviolite"
			species = "#{species}-Mega"
		end

		species = "#{species}-Mega-X" if item[-5..-1] == "ite X"
		species = "#{species}-Mega-Y" if item[-5..-1] == "ite Y"

		move1 = move_names[safe_parse_move(set_data[i + 3])] || ""
		move2 = move_names[safe_parse_move(set_data[i + 4])] || ""
		move3 = move_names[safe_parse_move(set_data[i + 5])] || ""
		move4 = move_names[safe_parse_move(set_data[i + 6])] || ""
		moves = [move1, move2, move3, move4]

		ability_line = nil
		(0..10).each do |offset|
		line_candidate = set_data[i + offset]
		next if line_candidate.nil?
		if line_candidate.strip.start_with?("ability ")
			ability_line = line_candidate.strip
			break
		end
		end

		ability = ability_line ? parse_ability(ability_line) : nil

		ivs_line = nil
		(0..10).each do |offset|
		line_candidate = set_data[i + offset]
		next if line_candidate.nil?
		if line_candidate.strip.start_with?("setivs ")
			ivs_line = line_candidate.strip
			break
		end
		end

		if ivs_line
		iv_values = ivs_line[6..].split(",").map(&:strip).map(&:to_i)
		ivs = {}
		(0..5).each do |n|
			ivs[stat_names[n]] = iv_values[n]
		end
		else
		# fallback or default ivs, or raise warning
		ivs = Hash[stat_names.map { |s| [s, 31] }]  # for example default 31 IVs
		end

		nature = nil
		(0..10).each do |offset|
		line_candidate = set_data[i + offset]
		next if line_candidate.nil?
		if line_candidate.strip.start_with?("nature ")
			# Extract the string after "nature "
			nature_str = line_candidate.strip[7..].strip  # remove "nature "
			# Remove "NATURE_" prefix if present
			nature_str = nature_str.sub(/^NATURE_/, "")
			# Capitalize the nature properly
			nature = nature_str.downcase.capitalize
			break
		end
		end

		# Default nature if none found
		nature ||= "Hardy"

		

		sets[species] ||= {}
		set_name = "Lvl #{level} #{current_tr_title}"
		sets[species][set_name] = {
			"ivs": ivs, 
			"item": item,
			"level": level,
			"moves": moves,
			"tr_id": current_tr_id,
			"nature": nature,
			"ability": ability,
			"sub_index": current_sub_index,
			"battle_type": current_battle_type,
		}
		current_sub_index += 1
	end 
	i += 1
	break if i >= 44644
end

i = 0
current_lvl = 1
current_water_lvl = 101
current_location = nil
current_section_name = nil
current_section_info = {}
current_section_index = 0

# Rates for grass and water encounters
grass_rates = [10,10,10,10,10,10,10,10,5,5,5,5]
water_rates = [40,20,20,10,10]

encs = {}    # Final encounters hash
poks ||= {}  # Your poks hash (make sure this is initialized before)
pok_names ||= {} # Your mapping from variable names to pokémon names

while i < enc_data.length
  line = enc_data[i].strip
  i += 1
  next if line.empty?

  # Detect new location header
  if line.include?("/************** ")
    # Get location name (everything between the asterisks except first token)
    parts = line.split(" ")
    current_location = parts[1..-2].join(" ").strip
    encs[current_location] ||= {}
    current_water_lvl = 101
    # Reset section info on new location
    current_section_info = {}
    current_section_index = 0
    current_section_name = nil
    next
  end

  # Detect section headers from comments
  if line.start_with?("// ")
    current_section_info = {}
    current_section_index = 0

    case line.downcase
    when /surf encounters/
      current_section_name = "surf"
    when /old rod/
      current_section_name = "rod"
    when /morning encounter slots/
      current_section_name = "morning"
    when /day encounter slots/
      current_section_name = "day"
    when /night encounter slots/
      current_section_name = "night"
    when /grass encounter slots/
      current_section_name = "grass"
    else
      current_section_name = nil
    end
    next
  end

  # Parse walklevels for grass levels
  if line.start_with?("walklevels")
    levels = line.split(" ", 2)[1].split(",").map(&:strip).map(&:to_i)
    # Use last level in walklevels unless 0, fallback 59 or 101
    lvl_candidate = levels.last || 59
    lvl_candidate = 59 if lvl_candidate == 0
    current_lvl = lvl_candidate > 0 ? lvl_candidate : 101
    encs[current_location] ||= {}
    encs[current_location]["grass_lvl"] = current_lvl
    next
  end

  # Skip unwanted sections by matching keywords
  if line.match?(/hoenn|sinnoh|rock smash/i)
    # Skip next 3 lines to avoid them
    i += 3
    next
  end

  if line.match?(/good rod|super rod/i)
    # Skip next 6 lines to avoid them
    i += 6
    next
  end

  if line.match?(/\/\/ swarm/i)
    i += 2
    next
  end

  # Process grass encounters (pokemon lines)
  if line.start_with?("pokemon")
    pok_variable_name = line.split.last
    pok_name = pok_names[pok_variable_name]

    next if pok_name.nil? || poks[pok_name].nil? || current_section_name.nil? || current_section_name != "grass"

    # Initialize if needed
    poks[pok_name]["eal"] ||= 101
    poks[pok_name]["eal"] = [poks[pok_name]["eal"], current_lvl].min

    if current_section_info[pok_name]
      current_section_info[pok_name]["rate"] += grass_rates[current_section_index] || 0
    else
      current_section_info[pok_name] = {"rate" => grass_rates[current_section_index] || 0, "index" => current_section_index}
    end

    enc_info = {"s" => pok_name, "rate" => current_section_info[pok_name]["rate"]}
    encs[current_location]["grass"] ||= []
    encs[current_location]["grass"][current_section_info[pok_name]["index"]] = enc_info

    current_section_index += 1
    next
  end

  # Process water encounters (encounter and encounterwithform)
  if line.start_with?("encounter")
    # Clean line to handle both "encounter" and "encounterwithform"
    is_with_form = line.start_with?("encounterwithform")
    parts = line.split(" ")

    # Extract species name and form/levels depending on type
	if is_with_form
	# Your existing logic here (make sure to handle parts carefully)
	else
		# safer extraction
		pok_variable_name = parts[1] ? parts[1].chomp(",") : nil
		water_level_min_str = parts[2]
		water_level_max_str = parts[3]

		water_level_min = water_level_min_str ? water_level_min_str.chomp(",").to_i : 0
		water_level_max = water_level_max_str ? water_level_max_str.to_i : water_level_min
		form = nil

		# skip processing if no pok_variable_name or water_level_min is zero (or invalid)
		next if pok_variable_name.nil? || pok_variable_name.empty? || water_level_min == 0
	end
    # Update rate
    if current_section_info[pok_name]
      current_section_info[pok_name]["rate"] += water_rates[current_section_index] || 0
    else
      current_section_info[pok_name] = {"rate" => water_rates[current_section_index] || 0, "index" => current_section_index}
    end

    enc_info = {
      "s" => pok_name,
      "rate" => current_section_info[pok_name]["rate"],
      "form" => form,
      "level_min" => water_level_min,
      "level_max" => water_level_max
    }

    poks[pok_name]["eal"] ||= 101
    poks[pok_name]["eal"] = [poks[pok_name]["eal"], water_level_min].min

    current_water_lvl = [water_level_min, current_water_lvl].min

    encs[current_location]["#{current_section_name}_lvl"] = current_water_lvl
    encs[current_location][current_section_name] ||= []
    encs[current_location][current_section_name][current_section_info[pok_name]["index"]] = enc_info

    current_section_index += 1
    next
  end

  # Other lines ignored
end

# Compact arrays to remove nil holes safely
formatted_encs = {}

encs.each do |loc, data|
  formatted_encs[loc] = data
  %w[grass surf rod morning day night].each do |section|
    if formatted_encs[loc].key?(section)
      formatted_encs[loc][section] = formatted_encs[loc][section].compact
    end
  end
end


i = 0
current_pok = ""
current_ls = []


ordered_items = get_constants "../asm/include/items.inc", "ITEM"
ordered_abilities = get_constants "../asm/include/abilities.inc", "ABILITY"
includes = {"poks" => ordered_poks, "moves" => ordered_moves, "abilities" => ordered_abilities, "items" => ordered_items, "growths" => ordered_growths}

while i < ls_data.length
	line = ls_data[i].strip

	if line.include?("levelup")
		pok_variable_name = line.split(" ")[1]
		pok_name = pok_names[pok_variable_name] 

		if !pok_name
			break
		end

		current_ls = []
		current_pok = pok_name
	end

	if line.include?("learnset MOVE")
		move_variable_name = line.split(" ")[1][0..-2]
		lvl_learned = line.split(", ")[-1].to_i
		current_ls << [lvl_learned, ordered_moves.index(move_names[move_variable_name])]
	end

	if line.include?("terminate")
		# p current_pok
		poks[current_pok]["lsi"] ||= {}
		poks[current_pok]["lsi"]["ls"] = current_ls 
	end

	i += 1
end


i = 0
current_tm = ""
while i < tm_data.length
	line = tm_data[i].strip

	if line.include?("MOVE_")
		move_variable_name = line.split(" ")[1]
		move_name = move_names[move_variable_name] 

		current_tm = move_name
	end

	if line.include?("SPECIES_")
		pok_variable_name = line.strip
		pok_name = pok_names[pok_variable_name]
		begin
			poks[pok_name]["lsi"]["tms"] ||= []
		rescue
			# p "Could not find #{pok_name}"
			i += 1
			next
		end
		poks[pok_name]["lsi"]["tms"] << ordered_moves.index(current_tm)
	end

	i += 1
end

i = 0
current_tutor = ""
while i < tutor_data.length
	line = tutor_data[i].strip

	if line.include?("MOVE_")
		move_variable_name = line.split(" ")[1]
		move_name = move_names[move_variable_name] 

		current_tutor = ordered_moves.index(move_name)
	end

	if line.include?("SPECIES_")
		pok_variable_name = line.strip
		pok_name = pok_names[pok_variable_name]
		begin
			poks[pok_name]["lsi"]["tutors"] ||= []
		rescue
			# p "Could not find #{pok_name}"
			i += 1
			next
		end
		poks[pok_name]["lsi"]["tutors"] << current_tutor
	end

	i += 1
end


npoint_data = {"poks" => poks, "moves" => npoint_moves, "formatted_sets" => sets, "includes" => includes, "encs" => encs}
File.write("./npoint.json", JSON.dump(npoint_data))










