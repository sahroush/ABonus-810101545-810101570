#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const int MINUTES_OF_A_HOUR = 60;
const int INTERVAL = 30;
const int MIN_TIME = 15;

const int FILE_NAME_ARGUMENT = 1;
const int ADDITIONAL_CHAR = 2;
const int START_TIME = 8 * MINUTES_OF_A_HOUR;
const int DOUBLE_DIGIT = 10;

const int LOCATION_NAME_INDEX = 0;
const int OPENING_TIME_INDEX = 1;
const int CLOSING_TIME_INDEX = 2;
const int RANK_INDEX = 3;

vector<int> Open_times;
vector<int> Close_times;

struct Locations {
  int number;
  string name;
  int opening_time;
  int closing_time;
  int rank;
};

int searcher(vector<string> vec, string element) {
  for (int i = 0; i < vec.size(); i++) {
    if (vec[i] == element)
      return i;
  }
  return -1;
}

vector<int> arrangment_function(vector<string> titles) {
  int name_index = searcher(titles, "name");
  int opentime_index = searcher(titles, "openingTime");
  int closetime_index = searcher(titles, "closingTime");
  int rank_index = searcher(titles, "rank");
  vector<int> title_arrangment = {name_index, opentime_index, closetime_index,
                                  rank_index};
  return title_arrangment;
}

vector<string> read_locations_data(string file_name) {
  vector<string> input_strings;
  ifstream file(file_name);
  string first_line;
  string temp_str;
  getline(file, first_line);
  while (getline(file, temp_str))
    input_strings.push_back(temp_str);

  return input_strings;
}

vector<vector<string>> split_input(vector<string> input_strings) {
  vector<vector<string>> input_table;
  string token;
  vector<string> temp_vec;
  for (int i = 0; i < input_strings.size(); i++) {

    stringstream S(input_strings[i]);
    while (getline(S, token, ',')) {
      temp_vec.push_back(token);
    }
    input_table.push_back(temp_vec);
    temp_vec.clear();
  }
  return input_table;
}

vector<int> create_time_vector(vector<vector<string>> input_table,
                               int opentime_index) {
  vector<int> result;
  string token;
  vector<int> temp;
  int location_number = input_table.size();
  for (int i = 0; i < location_number; i++) {
    stringstream S(input_table[i][opentime_index]);
    while (getline(S, token, ':')) {
      stringstream ss(token);
      int temp_clock = 0;
      ss >> temp_clock;
      temp.push_back(temp_clock);
    }
    int close_time = temp[0] * MINUTES_OF_A_HOUR + temp[1];
    temp.clear();
    result.push_back(close_time);
  }
  return result;
}

vector<Locations> put_input_to_struct(vector<vector<string>> input_table,
                                      vector<int> open_times,
                                      vector<int> close_times,
                                      vector<int> title_arrangment) {
  vector<Locations> input_structs;
  int string_num = input_table.size();
  for (int i = 0; i < string_num; i++) {
    vector<Locations> input_vector;
    input_structs.push_back(
        {i + 1, input_table[i][title_arrangment[LOCATION_NAME_INDEX]],
         open_times[i], close_times[i],
         stoi(input_table[i][title_arrangment[RANK_INDEX]])});
  }
  return input_structs;
}

int find_min(vector<int> vec) {
  vector<string> nearest_times;
  int length = vec.size();
  if (length > 1) {
    int tail = vec[length - 1];
    vec.pop_back();
    if (tail <= find_min(vec)) {
      return tail;
    } else
      return find_min(vec);
  } else
    return vec[0];
}
bool int_finder(vector<int> vec, int element) {
  for (int i = 0; i < vec.size(); i++) {
    if (vec[i] == element)
      return true;
  }
  return false;
}
void find_suitable_indexs(vector<Locations> input, int nearest_time,
                          vector<int> location_check,
                          vector<int> &suitable_indexs,
                          vector<int> unsuitable_indexs) {
  // we find the next suitable indexes and put them in suitable
  // indexes, since we change the vector we pass it with & (i.e. pass by
  // reference) so the changes will apply to the original
  int size = input.size();
  for (int i = 0; i < size; i++) {
    if (input[i].opening_time == nearest_time) {
      bool existence_checker =
          int_finder(location_check, (input[i].number - 1));
      bool unsuitable_checker = int_finder(unsuitable_indexs, i);
      if (!existence_checker && !unsuitable_checker)
        suitable_indexs.push_back(i);
    }
  }
}

int match_num_rank(vector<Locations> input, int rank) {
  for (int i = 0; i < input.size(); i++) {
    if (input[i].rank == rank)
      return i;
  }
  return -1;
}
int find_best(vector<int> suitable_indexs, vector<Locations> input) {
  vector<int> ranks;
  int index;
  int size = suitable_indexs.size();
  for (int i = 0; i < size; i++) {
    index = suitable_indexs[i];
    ranks.push_back(input[index].rank);
  }
  int best = find_min(ranks);
  return best;
}

int calculate(int previous_time, int duration) {
  if (duration >= MINUTES_OF_A_HOUR)
    duration = MINUTES_OF_A_HOUR;
  return previous_time + duration + INTERVAL;
}
int calculate_endtime(int previous_time, int duration) {
  if (duration >= MINUTES_OF_A_HOUR)
    duration = MINUTES_OF_A_HOUR;
  return previous_time + duration;
}

vector<int> find_open_locations(vector<int> open_times, int current_time,
                                vector<int> location_check,
                                vector<int> unsuitable_indexs) {
  vector<int> suitable_indexs;
  for (int i = 0; i < open_times.size(); i++) {
    int existence_checker = int_finder(location_check, i);
    if (open_times[i] <= current_time && existence_checker == 0) {
      int unsuitable_check = int_finder(unsuitable_indexs, i);
      if (unsuitable_check == 0)
        suitable_indexs.push_back(i);
    }
  }
  return suitable_indexs;
}
int find_nearest(int current_time, vector<int> opentimes) {
  vector<int> late_opentimes;
  for (int i = 0; i < opentimes.size(); i++) {
    if (opentimes[i] > current_time) {
      late_opentimes.push_back(opentimes[i]);
    }
  }
  int min = find_min(late_opentimes);
  return min;
}

int find_next_destination_index(int &current_time, vector<int> open_times,
                                vector<Locations> input,
                                vector<int> location_check,
                                vector<int> unsuitable_indexs) {
  vector<int> suitable_indexs;
  int rank, index;
  int counter = 0;
  int len = open_times.size();
  while (true) {
    // we stay in this while till suitable indexes is empty
    suitable_indexs.clear();
    if (current_time >= find_min(open_times) && counter < len) {
      suitable_indexs = find_open_locations(open_times, current_time,
                                            location_check, unsuitable_indexs);
      counter += 1;
    } else {
      vector<int> late_opentimes;
      for (int i = 0; i < open_times.size(); i++) {
        if (open_times[i] > current_time)
          late_opentimes.push_back(open_times[i]);
      }
      int nearest_time = find_min(late_opentimes);
      find_suitable_indexs(input, nearest_time, location_check, suitable_indexs,
                           unsuitable_indexs);
      current_time = nearest_time;
    }
    if (suitable_indexs.size() > 0) {
      rank = find_best(suitable_indexs, input);
      index = match_num_rank(input, rank);
      break;
    }
  }

  return index;
}

int check_destination_wellness(vector<Locations> input, int current_time,
                               int index) {
  int duration = input[index].closing_time - current_time;
  if (duration > MIN_TIME)
    return duration;
  else
    return -1;
}

int find_max(vector<int> closing_time) {
  int size = closing_time.size();
  if (size == 1)
    return closing_time[0];
  else {
    int tail = closing_time[size - 1];
    closing_time.pop_back();
    if (tail >= find_max(closing_time))
      return tail;
    else
      return find_max(closing_time);
  }
}

int existence_check(vector<int> location_check, int index) {
  for (int i = 0; i < location_check.size(); i++) {
    if (location_check[i] == index)
      return i;
  }
  return -1;
}
void find_next_destenation(int current_time, vector<Locations> input,
                           vector<int> &location_check, vector<int> &start,
                           vector<int> &durations) {
  // TODO: fix this function
  int size = input.size();
  vector<int> not_suitables;
  int counter = 0;
  while (current_time < find_max(Close_times) && counter < size) {
    int index = find_next_destination_index(current_time, Open_times, input,
                                            location_check, not_suitables);
    int existence_checker = existence_check(location_check, index);
    int duration_check = check_destination_wellness(input, current_time, index);
    if (existence_checker == (-1) && duration_check != (-1)) {
      location_check.push_back(index);
      start.push_back(current_time);
      durations.push_back(duration_check);
      current_time = calculate(current_time, duration_check);
    } else
      not_suitables.push_back(index);
    counter += 1;
  }
}
string convert_int_to_clockform(int time) {
  int hour = time / MINUTES_OF_A_HOUR;
  int min = time - (hour * MINUTES_OF_A_HOUR);
  stringstream ss;
  ss << hour;
  string hour_str = ss.str();
  stringstream sss;
  sss << min;
  string min_str = sss.str();
  if (hour < DOUBLE_DIGIT && min < DOUBLE_DIGIT)
    return "0" + hour_str + ":" + "0" + min_str;
  else if (hour < DOUBLE_DIGIT && min >= DOUBLE_DIGIT)
    return "0" + hour_str + ":" + min_str;
  else if (hour >= DOUBLE_DIGIT && min < DOUBLE_DIGIT)
    return hour_str + ":" + "0" + min_str;
  else
    return hour_str + ":" + min_str;
}

vector<vector<string>> make_vector_ready_for_print(vector<Locations> input,
                                                   vector<int> close_times,
                                                   vector<int> &location_check,
                                                   vector<int> &start,
                                                   vector<int> &durations) {
  // in this function we are gonna make the
  // vector ready for print
  vector<vector<string>> result;
  for (int i = 0; i < location_check.size(); i++) {
    int end = calculate_endtime(start[i], durations[i]);
    string standard_start = convert_int_to_clockform(start[i]);
    string standard_end = convert_int_to_clockform(end);
    vector<string> temp = {input[location_check[i]].name, standard_start,
                           standard_end};
    result.push_back(temp);
  }
  return result;
}

void print_output(vector<vector<string>> result) {
  for (int i = 0; i < result.size(); i++) {
    cout << "Location " << result[i][LOCATION_NAME_INDEX] << endl
         << "Visit from " << result[i][OPENING_TIME_INDEX] << " until "
         << result[i][CLOSING_TIME_INDEX] << endl
         << "---" << endl;
  }
}

vector<Locations> read_from_file(string file_name) {
  // this function reads input from a file

  ifstream file(file_name);
  string temp_str;
  getline(file, temp_str);

  vector<string> splitted_firstline;
  string token;
  stringstream S(temp_str);
  while (getline(S, token, ','))
    splitted_firstline.push_back(token);

  vector<int> arrangment = arrangment_function(splitted_firstline);
  vector<string> primitive_get = read_locations_data(file_name);
  vector<vector<string>> splitted_input = split_input(primitive_get);

  Open_times =
      create_time_vector(splitted_input, arrangment[OPENING_TIME_INDEX]);
  Close_times =
      create_time_vector(splitted_input, arrangment[CLOSING_TIME_INDEX]);
  vector<Locations> location_data =
      put_input_to_struct(splitted_input, Open_times, Close_times, arrangment);
  return location_data;
}

int main(int argc, char *argv[]) {

  vector<int> gone_location;
  vector<int> start_times;
  vector<int> durations;
  vector<Locations> location_data =
      read_from_file(argv[FILE_NAME_ARGUMENT] + ADDITIONAL_CHAR);
  find_next_destenation(START_TIME, location_data, gone_location, start_times,
                        durations);
  vector<vector<string>> ready_to_print = make_vector_ready_for_print(
      location_data, Close_times, gone_location, start_times, durations);
  print_output(ready_to_print);
}
