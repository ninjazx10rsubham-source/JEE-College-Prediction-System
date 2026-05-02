function estimatePercentile(marks){

    if(marks >= 290) return 99.99
    if(marks >= 250) return 99.5
    if(marks >= 220) return 99.0
    if(marks >= 180) return 97.0
    if(marks >= 150) return 95.0
    if(marks >= 120) return 90.0
    if(marks >= 100) return 85.0
    if(marks >= 80) return 75.0

    return 60.0
}

function estimateRank(marks){

    if(marks >= 290) return 1
    if(marks >= 250) return 500
    if(marks >= 220) return 5000
    if(marks >= 180) return 15000
    if(marks >= 150) return 30000
    if(marks >= 120) return 60000
    if(marks >= 100) return 90000
    if(marks >= 80) return 120000

    return 200000
}

function performanceBand(percentile){

    if(percentile >= 99.9)
        return "EXCELLENT – Top 0.1%"

    if(percentile >= 99.0)
        return "VERY GOOD – Top 1%"

    if(percentile >= 95.0)
        return "GOOD – Top 5%"

    if(percentile >= 90.0)
        return "ABOVE AVERAGE"

    if(percentile >= 75.0)
        return "AVERAGE"

    return "BELOW AVERAGE"
}

function predict(){

    const marks =
        parseInt(document.getElementById("marks").value)

    const category =
        document.getElementById("category").value

    if(isNaN(marks) || marks < 0 || marks > 300){

        alert("Enter valid marks between 0 and 300")

        return
    }

    const percentile =
        estimatePercentile(marks)

    const rank =
        estimateRank(marks)

    const band =
        performanceBand(percentile)

    let colleges = []

    if(rank <= 5000){

        colleges.push("NIT Trichy - CSE")
        colleges.push("NIT Surathkal - CSE")
    }

    else if(rank <= 15000){

        colleges.push("NIT Warangal - ECE")
        colleges.push("NIT Allahabad - IT")
    }

    else if(rank <= 30000){

        colleges.push("NIT Bhopal - Mechanical")
        colleges.push("NIT Jaipur - Civil")
    }

    else if(rank <= 60000){

        colleges.push("IIIT Lucknow")
        colleges.push("NIT Agartala")
    }

    else{

        colleges.push("State Government Colleges")
        colleges.push("Private Engineering Colleges")
    }

    let html = `

    <div class="result-box">

        <h2>Prediction Result</h2>

        <p><strong>Marks:</strong> ${marks}</p>

        <p><strong>Category:</strong> ${category}</p>

        <p><strong>Estimated Percentile:</strong>
        ${percentile}</p>

        <p><strong>Estimated Rank:</strong>
        ${rank}</p>

        <p><strong>Performance:</strong>
        ${band}</p>

        <h2 style="margin-top:20px;">
        Predicted Colleges
        </h2>
    `

    colleges.forEach(college => {

        html += `

        <div class="college">

            <h3>${college}</h3>

        </div>
        `
    })

    html += `</div>`

    document.getElementById("result").innerHTML = html
}

const themeBtn =
    document.getElementById("themeBtn")

let darkMode = false

themeBtn.addEventListener("click", () => {

    darkMode = !darkMode

    document.body.classList.toggle("dark-mode")

    if(darkMode){

        themeBtn.innerText = "☀️ Light Mode"
    }
    else{

        themeBtn.innerText = "🌙 Dark Mode"
    }
})
