document.getElementById('treatment-form').addEventListener('submit', function(e) {
    e.preventDefault();

    const formData = new FormData(e.target);
    const data = Object.fromEntries(formData.entries());

    fetch('/api/treatment', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    })
    .then(response => response.json())
    .then(result => {
        document.getElementById('initial-schedule').textContent = result.initial_schedule;
        document.getElementById('optimized-schedule').textContent = result.optimized_schedule;
        document.getElementById('result').style.display = 'block';
    })
    .catch(error => console.error('Error:', error));
});