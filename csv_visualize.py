import csv

import matplotlib.pyplot as plt
form sys import argv

if __name__ == '__main__':
    with open(argv[0], newline='') as file:
        dict_list = list(csv.DictReader(file))

    ks = [
        5,
        10,
        20,
        50,
        100,
        200]
    genome_sizes = [
            # 500,
            # 1000,
            # 5000,
            10000]

    for genome_size in genome_sizes:
        lookups = []
        for k in ks:
            lookup = list(
                filter(lambda x: x['genome_size'] == str(genome_size)
                       and x['k'] == str(k),
                    dict_list))

            lookup = lookup[0]
            lookups.append(lookup['lookup_ms'])

        plt.title(f'Trajanje lookup operacije')
        # print(ks)
        # print(lookups)
        plt.xlabel('Duljina k-mera')
        plt.ylabel('Trajanje pretraživanja [ms]')
        plt.plot(ks, lookups)
        plt.show()

    insertion_times = []
    memory_usages = []
    levels = []
    for k in ks:
        entry = list(
                filter(lambda x: x['k'] == str(k) and
                       x['genome_size'] == str(genome_sizes[0]),
                       dict_list
                       ))

        assert(len(entry) == 1)
        entry = entry[0]

        insertion_times.append(
                entry['insertion_ms']
            )
        memory_usages.append(
                entry['memory_kb']
                )
        levels.append(
                entry['levels']
                )

    # graf za insertion time
    plt.title('Insertion time')
    plt.plot(ks, insertion_times)
    plt.show()

    # graf memorije
    plt.title('Koristena memorija')
    plt.plot(ks, memory_usages)
    plt.show()

    # graf elemenata
    plt.title('Broj razina LDCF strukture')
    plt.plot(ks, levels, 'bo')
    plt.show()
