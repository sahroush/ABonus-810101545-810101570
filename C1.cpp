#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const int MINUTES_OF_AN_HOUR = 60;
const int INTERVAL = 30;
const int MIN_TIME = 15;

const int FILE_NAME_ARGUMENT = 1;
const int ADDITIONAL_CHAR_CNT = 2;
const int START_TIME = 8 * MINUTES_OF_AN_HOUR;
const int DOUBLE_DIGIT = 10;

const int LOCATION_NAME_INDEX = 0;
const int OPENING_TIME_INDEX = 1;
const int CLOSING_TIME_INDEX = 2;
const int RANK_INDEX = 3;

struct WorkingHours {
  int open_time, close_time;
};

typedef vector<WorkingHours> TimeList;
typedef vector<vector<string>> Table;

struct Location {
  int number;
  string name;
  int opening_time;
  int closing_time;
  int rank;
  Location(int number, string name, WorkingHours wh, int rank)
      : number(number), name(name), opening_time(wh.open_time),
        closing_time(wh.close_time), rank(rank){};
};

typedef vector<Location> LocationList;

int searcher(vector<string> vec, string element) {
  for (int i = 0; i < (int)vec.size(); i++) {
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

vector<string> read_Location_data(string file_name) {
  vector<string> input_strings;
  ifstream file(file_name);
  string first_line;
  string temp_str;
  getline(file, first_line);
  while (getline(file, temp_str))
    input_strings.push_back(temp_str);
  return input_strings;
}

Table split_input(vector<string> input_strings) {
  Table input_table;
  string token;
  vector<string> temp_vec;
  for (int i = 0; i < (int)input_strings.size(); i++) {

    stringstream S(input_strings[i]);
    while (getline(S, token, ',')) {
      temp_vec.push_back(token);
    }
    input_table.push_back(temp_vec);
    temp_vec.clear();
  }
  return input_table;
}

int get_time(string time_str) {
  stringstream sstr(time_str);
  vector<int> temp;
  string token;
  while (getline(sstr, token, ':')) {
    stringstream ss(token);
    int temp_clock = 0;
    ss >> temp_clock;
    temp.push_back(temp_clock);
  }
  int res = temp[0] * MINUTES_OF_AN_HOUR + temp[1];
  return res;
}

TimeList create_time_vector(Table input_table,
                            int starting_time_id, int finishing_time_id) {
  TimeList result;
  int location_number = input_table.size();
  for (int i = 0; i < location_number; i++) {
    result.push_back({get_time(input_table[i][starting_time_id]),
                      get_time(input_table[i][finishing_time_id])});
  }
  return result;
}

LocationList make_Location(Table input_table,
                                 TimeList times, vector<int> title_arrangment) {
  LocationList input_structs;
  int location_count = input_table.size();
  for (int i = 0; i < location_count; i++) {
    input_structs.push_back(
        {i + 1, input_table[i][title_arrangment[LOCATION_NAME_INDEX]], times[i],
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
  for (int i = 0; i < (int)vec.size(); i++) {
    if (vec[i] == element)
      return true;
  }
  return false;
}
void find_suitable_indexs(LocationList input, int nearest_time,
                          vector<int> location_check,
                          vector<int> &suitable_indexs,
                          vector<int> unsuitable_indexs) {
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

int match_num_rank(LocationList input, int rank) {
  for (int i = 0; i < (int)input.size(); i++) {
    if (input[i].rank == rank)
      return i;
  }
  return -1;
}
int find_best(vector<int> suitable_indexs, LocationList input) {
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
  if (duration >= MINUTES_OF_AN_HOUR)
    duration = MINUTES_OF_AN_HOUR;
  return previous_time + duration + INTERVAL;
}
int calculate_endtime(int previous_time, int duration) {
  if (duration >= MINUTES_OF_AN_HOUR)
    duration = MINUTES_OF_AN_HOUR;
  return previous_time + duration;
}

vector<int> find_open_Location(TimeList times, int current_time,
                                vector<int> location_check,
                                vector<int> unsuitable_indexs) {
  vector<int> suitable_indexs;
  int count = times.size();
  for (int i = 0; i < count; i++) {
    int existence_checker = int_finder(location_check, i);
    if (times[i].open_time <= current_time && existence_checker == 0) {
      int unsuitable_check = int_finder(unsuitable_indexs, i);
      if (unsuitable_check == 0)
        suitable_indexs.push_back(i);
    }
  }
  return suitable_indexs;
}
int find_nearest(int current_time, vector<int> opentimes) {
  vector<int> late_opentimes;
  for (int i = 0; i < (int)opentimes.size(); i++) {
    if (opentimes[i] > current_time) {
      late_opentimes.push_back(opentimes[i]);
    }
  }
  int min = find_min(late_opentimes);
  return min;
}

bool compare_by_open_time(WorkingHours a, WorkingHours b) {
  return a.open_time < b.open_time;
}

int find_next_destination_index(int &current_time, TimeList times,
                                LocationList input,
                                vector<int> location_check,
                                vector<int> unsuitable_indexs) {
  vector<int> suitable_indexs;
  int rank, index;
  int counter = 0;
  int len = times.size();
  while (true) {
    // we stay in this while till suitable indexes is empty
    suitable_indexs.clear();
    if (current_time >=
            min_element(times.begin(), times.end(), compare_by_open_time)->open_time &&
        counter < len) {
      suitable_indexs = find_open_Location(times, current_time, location_check,
                                            unsuitable_indexs);
      counter += 1;
    } else {
      vector<int> late_opentimes;
      for (int i = 0; i < len; i++) {
        if (times[i].open_time > current_time)
          late_opentimes.push_back(times[i].open_time);
      }
      int nearest_time =
          *min_element(late_opentimes.begin(), late_opentimes.end());
      find_suitable_indexs(input, nearest_time, location_check, suitable_indexs,
                           unsuitable_indexs);
      current_time = nearest_time;
    }
    if (!suitable_indexs.empty()) {
      rank = find_best(suitable_indexs, input);
      index = match_num_rank(input, rank);
      break;
    }
  }
  return index;
}

int check_destination_wellness(LocationList input, int current_time,
                               int index) {
  int duration = input[index].closing_time - current_time;
  if (duration > MIN_TIME)
    return duration;
  else
    return -1;
}

int find_max(TimeList times, int order) {
  int mx = -100;
  for (WorkingHours wh : times) {
    mx =
        max(mx, ((order == CLOSING_TIME_INDEX) ? wh.close_time : wh.open_time));
  }
  return mx;
}

int existence_check(vector<int> location_check, int index) {
  for (int i = 0; i < (int)location_check.size(); i++) {
    if (location_check[i] == index)
      return i;
  }
  return -1;
}

bool compare_by_end_time(WorkingHours a, WorkingHours b) {
  return a.close_time < b.close_time;
}

void process_input(int current_time, LocationList input,
                           vector<int> &location_check, vector<int> &start,
                           vector<int> &durations, TimeList times) {
  int size = input.size();
  vector<int> not_suitables;
  int counter = 0;
  while (current_time <
             max_element(times.begin(), times.end(), compare_by_end_time)->close_time &&
         counter < size) {
    int index = find_next_destination_index(current_time, times, input,
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
  int hour = time / MINUTES_OF_AN_HOUR;
  int min = time - (hour * MINUTES_OF_AN_HOUR);
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

Table generate_output(LocationList input,
                                                   vector<int> &location_check,
                                                   vector<int> &start,
                                                   vector<int> &durations) {
  Table result;
  for (int i = 0; i < (int)location_check.size(); i++) {
    int end = calculate_endtime(start[i], durations[i]);
    string standard_start = convert_int_to_clockform(start[i]);
    string standard_end = convert_int_to_clockform(end);
    vector<string> temp = {input[location_check[i]].name, standard_start,
                           standard_end};
    result.push_back(temp);
  }
  return result;
}

void print_output(Table result) {
  for (int i = 0; i < (int)result.size(); i++) {
    cout << "Location " << result[i][LOCATION_NAME_INDEX] << endl
         << "Visit from " << result[i][OPENING_TIME_INDEX] << " until "
         << result[i][CLOSING_TIME_INDEX] << endl
         << "---" << endl;
  }
}

string read_line(ifstream &file) {
  string res;
  getline(file, res);
  return res;
}

vector<string> split_by_comma(string inp) {
  vector<string> ans;
  string token;
  stringstream S(inp);
  while (getline(S, token, ','))
    ans.push_back(token);
  return ans;
}

vector<string> handle_first_line(ifstream &file) {
  string first_line = read_line(file);
  return split_by_comma(first_line);
}

LocationList read_input(string file_name,
                                 LocationList &location_data,
                                 TimeList &times) {
  ifstream file(file_name);
  vector<string> splitted_firstline = handle_first_line(file);

  vector<int> arrangment = arrangment_function(splitted_firstline);
  vector<string> primitive_get = read_Location_data(file_name);
  Table splitted_input = split_input(primitive_get);

  times = create_time_vector(splitted_input, arrangment[OPENING_TIME_INDEX],
                             arrangment[CLOSING_TIME_INDEX]);
  location_data = make_Location(splitted_input, times, arrangment);
  return location_data;
}

int main(int argc, char *argv[]) {
  vector<int> gone_location;
  vector<int> start_times;
  vector<int> durations;
  LocationList location_data;
  TimeList times;
  read_input(argv[FILE_NAME_ARGUMENT] + ADDITIONAL_CHAR_CNT, location_data,
                 times);
  process_input(START_TIME, location_data, gone_location, start_times,
                        durations, times);
  Table output = generate_output(
      location_data, gone_location, start_times, durations);
  print_output(output);
}
