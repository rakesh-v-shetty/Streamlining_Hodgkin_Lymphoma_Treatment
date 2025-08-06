from flask import Flask, render_template, request, redirect, url_for, session
import random
import math

app = Flask(__name__)
app.secret_key = 'your_secret_key'  # Change this to a secure key in production

class PatientData:
    def __init__(self, stage, heart_health, lung_health, bulky_tumors, num_cycles=0, weeks_per_cycle=0, break_weeks=0, treatment_days=0, break_days=0):
        self.stage = stage
        self.heart_health = heart_health
        self.lung_health = lung_health
        self.bulky_tumors = bulky_tumors
        self.num_cycles = num_cycles
        self.weeks_per_cycle = weeks_per_cycle
        self.break_weeks = break_weeks
        self.treatment_days = treatment_days
        self.break_days = break_days

def choose_treatment_method(data):
    if data.stage <= 2 and data.heart_health > 60 and data.lung_health > 60 and not data.bulky_tumors:
        data.num_cycles = 6
        data.weeks_per_cycle = 4
        data.break_weeks = 1
    elif data.stage <= 2 and data.heart_health > 50 and data.lung_health > 50 and data.bulky_tumors:
        data.num_cycles = 12
        data.weeks_per_cycle = 3
        data.break_weeks = 0
    elif data.stage > 2 and data.heart_health > 50 and data.lung_health > 50:
        data.num_cycles = 4
        data.weeks_per_cycle = 6
        data.break_weeks = 2
    else:
        data.num_cycles = 8
        data.weeks_per_cycle = 3
        data.break_weeks = 1

def generate_initial_schedule(data):
    data.treatment_days = data.weeks_per_cycle * 5  # Assume treatment on weekdays
    data.break_days = data.weeks_per_cycle * 2      # Assume break on weekends

def cost_function(data):
    return data.treatment_days * 1.0 + data.break_days * 0.5

def simulated_annealing(data):
    temp = 1000.0
    cooling_rate = 0.95
    max_iterations = 1000

    current_data = data
    best_data = data
    best_cost = cost_function(best_data)

    for _ in range(max_iterations):
        temp *= cooling_rate

        new_data = PatientData(
            stage=current_data.stage,
            heart_health=current_data.heart_health,
            lung_health=current_data.lung_health,
            bulky_tumors=current_data.bulky_tumors,
            num_cycles=current_data.num_cycles,
            weeks_per_cycle=current_data.weeks_per_cycle,
            break_weeks=current_data.break_weeks,
            treatment_days=current_data.treatment_days + random.randint(-2, 2),
            break_days=current_data.break_days + random.randint(-2, 2)
        )

        if new_data.treatment_days < 0: new_data.treatment_days = 0
        if new_data.break_days < 0: new_data.break_days = 0

        new_cost = cost_function(new_data)

        if new_cost < best_cost or (math.exp((best_cost - new_cost) / temp) > random.random()):
            current_data = new_data
            if new_cost < best_cost:
                best_data = new_data
                best_cost = new_cost

    return best_data

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        stage = int(request.form['stage'])
        heart_health = int(request.form['heart_health'])
        lung_health = int(request.form['lung_health'])
        bulky_tumors = int(request.form['bulky_tumors'])

        data = PatientData(stage, heart_health, lung_health, bulky_tumors)
        choose_treatment_method(data)
        generate_initial_schedule(data)

        session['patient_data'] = data.__dict__

        return redirect(url_for('treatment_results'))

    return render_template('index.html')

@app.route('/treatment_results', methods=['GET', 'POST'])
def treatment_results():
    data = PatientData(**session['patient_data'])

    if request.method == 'POST':
        tolerance = int(request.form['tolerance'])
        recovery = int(request.form['recovery'])
        wbc = int(request.form['wbc'])
        rbc = int(request.form['rbc'])
        platelet_count = int(request.form['platelet_count'])
        tumor_change = int(request.form['tumor_change'])
        heart_health = int(request.form['heart_health'])
        lung_health = int(request.form['lung_health'])

        if tolerance < 50 or recovery < 50 or wbc < 4 or rbc < 3 or platelet_count < 100 or heart_health < 50 or lung_health < 50:
            data.treatment_days -= 2
            data.break_days += 2
        elif tumor_change < 0 and tolerance > 70 and recovery > 70:
            data.treatment_days += 1
            data.break_days -= 1

        if data.treatment_days < 0: data.treatment_days = 0
        if data.break_days < 0: data.break_days = 0

        session['patient_data'] = data.__dict__

        return redirect(url_for('treatment_results'))

    return render_template('treatment_results.html', data=data)

if __name__ == '__main__':
    app.run(debug=True)