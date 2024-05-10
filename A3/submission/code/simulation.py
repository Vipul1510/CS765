import random
import matplotlib.pyplot as plt
import argparse

def simulate_behaviour(iterations, N, p, q):
    # Initialize trust scores as 128 for all voters
    trust_scores = [128] * N
    # Initialize trust count as 0 for all voters
    trust_count = [0] * N

    # Calculate counts to track number of each type of voter
    very_honest_count = int(N * (1 - q) * p)
    less_honest_count = int(N * (1 - q) * (1 - p))
    malicious_count = int(N * q)

    # Lists to store average trustworthiness of voters for each category
    avg_very_honest = []
    avg_less_honest = []
    avg_malicious = []
    news_verified = []

    # Simulatimg voting behaviour for each news item
    for i in range(iterations):

        votes = [] # Initialize empty vote list
        news_score = 0 # This will be updated by each voter

        # Use random numbers to predict votes of different voters using given probability distributions
        # Create a random news with correctness of 0.5
        news_real_check =1 if random.random()<0.5 else 0


        ### Voting Starts ###
        # Each voter now assigns/ gives it's vote
        for j in range(N):
            if j < very_honest_count:                       ## Very honest voter
                probability_correct = 0.9
            elif j < very_honest_count + less_honest_count: ## Less honest voter
                probability_correct = 0.7
            else:                                           ## Malicious voter
                probability_correct = 0 

            vote = news_real_check if random.random() < probability_correct else 1- news_real_check
            votes.append(vote)
            news_score += vote* trust_scores[j]
        ### Voting Ends ###

        # Predicting correctness of votes using votes of voters
        news_final_score = news_score/sum(trust_scores)
        news_prediction = 1
        if(news_final_score<0.5):
            news_prediction = 0

        # Updating Trustworthiness value for each voter based on their vote given and majority vote
        for j in range(N):
            if votes[j] == news_prediction:
                trust_count[j]+=1
            # calculating factor f which belongs to (0,8192]
            factor =8192//(0.125*i + 1)
            # Weight update rule in simplified form => (1-f)*(Trust till now) + 0.5*f and f belongs to 0,1 here simplified form
            # Updating the trust score
            trust_scores[j] = ((8192-factor)*(255*trust_count[j])//(i+1) + 128*factor)//8192

        # Calculate average trustworthiness for each type of voter
        avg_very_honest.append(sum(trust_scores[:very_honest_count]) / (256*very_honest_count))
        avg_less_honest.append(sum(trust_scores[very_honest_count:very_honest_count + less_honest_count]) / (256*less_honest_count))
        avg_malicious.append(sum(trust_scores[very_honest_count + less_honest_count:]) / (256*malicious_count))

        # Keep track of the number of news verified
        news_verified.append(i + 1)

    return avg_very_honest, avg_less_honest, avg_malicious, news_verified

# This function simulates the behaviour for 200 news items and generates the plot
def Simulation(N, p, q):
    iterations = 200  # Number of news items
    # simulate the behaviour
    avg_very_honest, avg_less_honest, avg_malicious, news_verified = simulate_behaviour(iterations, N, p, q)
    # Plotting the Average Trustworthiness Vs News Verified plot
    plt.plot(news_verified, avg_very_honest, color='red', label='Very Honest')
    plt.plot(news_verified, avg_less_honest, color='green', label='Less Honest')
    plt.plot(news_verified, avg_malicious, color='blue', label='Malicious')
    plt.title(f'Average Trustworthiness Vs News Verified for N={N}, p={p}, q={q}')
    plt.xlabel('Number of News Verified')
    plt.ylabel('Average Trustworthiness')
    plt.legend()
    plt.grid(True)
    plt.savefig(f"plot_{N}_{p}_{q}.png")

# Parser to parse the command line arguments
parser = argparse.ArgumentParser()
parser.add_argument('--numvoters', type=int)
parser.add_argument('--fractionlesshonest', type=float)
parser.add_argument('--fractionmalicious', type=float)
args = parser.parse_args()

# Checking required parsers are present or not
if args.numvoters is None:
    print("Argument NUMVOTERS missing")
    exit(0)
if args.fractionlesshonest is None:
    print("Argument FRACTIONLESSHONEST missing")
    exit(0)
if args.fractionmalicious is None:
    print("Argument FRACTIONMALICIOUS missing")
    exit(0)

# Simualtion
Simulation(args.numvoters, args.fractionlesshonest, args.fractionmalicious)

