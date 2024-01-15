import random
import string
import time
import csv


FILE_PATH = "WordsList.txt"
ANSWER_FILE_PATH = "Answer.txt"

SEED = 3
random.seed(SEED)

RND_WORD_LEN_LOWER = 1
RND_WORD_LEN_UPPER = 100
WORD_COUNT = 100

WORD_MAX_LEN = 50

SEPARATOR = " "


def random_word() -> str:
  length = random.randint(1, WORD_MAX_LEN)
  word = ''.join(random.choice(string.ascii_letters) for _ in range(length))

  return word


def create_random_word_file(log = False):
  random_count_list = [random.randint(RND_WORD_LEN_LOWER, RND_WORD_LEN_UPPER) for _ in range(WORD_COUNT)]
  random_word_list = [random_word() for _ in range(WORD_COUNT)]

  if log:
    tuples = zip(random_word_list[:], random_count_list[:])
    start_time = time.time()

  with open(FILE_PATH, "w") as file:
    finished = False
    while not finished:
      indexes = [index for index, value in enumerate(random_count_list) if value > 0]
      random_index = random.choice(indexes)

      file.write(random_word_list[random_index] + ("" if len(indexes) == 1 and random_count_list[random_index] == 1 else SEPARATOR))
      random_count_list[random_index] -= 1

      nonzero_count = sum(1 for element in random_count_list if element > 0)

      if nonzero_count == 0:
        finished = True
  
  if log:
    print(f"Elapsed time: {time.time() - start_time:.3f}s.")

  with open(ANSWER_FILE_PATH, 'w', newline='') as csv_file:
    csv_writer = csv.writer(csv_file)
    csv_writer.writerows(tuples)


if __name__ == "__main__":
  create_random_word_file(log = True)