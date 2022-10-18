from queue import Queue
import threading
import os
import time

locker = threading.Lock()
storage = []

def collatz_function(num:int):
    if num % 2 == 0:
        time.sleep(0.001)
        return num // 2
    else:
        time.sleep(0.001)
        return (3*num+1) // 2

class element:
    def __init__(self, initial_number):
        self.initial_number = initial_number
        self.iterational_number = initial_number
        self.iteration = 0
        self.highest_point = initial_number
    def __str__(self):
        return "number: {} \titerations: {} \thighest_point: {}".format(self.initial_number, self.iteration, self.highest_point)
    def collatz_function(self):
        self.iterational_number = collatz_function(self.iterational_number)
        if self.iterational_number > self.highest_point:
            self.highest_point = self.iterational_number
        self.iteration += 1
    def possible_to_iterate(self):
        return self.iterational_number != 1

def proceeding(q:Queue):
    while not q.empty():
        locker.acquire()
        try:
            el = q.get()
        finally:
            locker.release()
        
        el.collatz_function()

        if el.possible_to_iterate():
            locker.acquire()
            try:
                q.put(el)
            finally:
                locker.release()
        else:
            storage.append(el)

def screen_clear():
    if os.name == 'posix':
        os.system('clear')
    else:
        os.system('cls')

if __name__ == "__main__":
    number_of_threads =  int(input("Enter number threads: "))
    number_of_elements = int(input("Enter number N for range [2, N]: "))
    q = Queue(maxsize=number_of_elements-1)

    for i in range(2, number_of_elements+1):
        el = element(i)
        q.put(el)

    screen_clear()

    threads = []

    start = time.time()
    for i in range(number_of_threads):
        threads.append(threading.Thread(target=proceeding, args=(q,)))
        threads[i].start()
        threads[i].join()
    end = time.time()
    
    for el in storage:
        print(el)
    print("\n\tNumber of threads: {}\n\tNumber of elements: {}\n\tTime taken: {}".format(number_of_threads, number_of_elements, end-start))