function predict(){

  const marks =
    Number(document.getElementById("marks").value)

  const category =
    document.getElementById("category").value

  if(marks < 0 || marks > 300){

    alert("Enter valid marks between 0 and 300")

    return
  }

  let closest = marksData[0]

  for(let data of marksData){

    if(
      Math.abs(data.marks - marks)
      <
      Math.abs(closest.marks - marks)
    ){
      closest = data
    }
  }

  let html = `

    <div class="result-box">

      <h2>Prediction Result</h2>

      <p>
      <strong>Marks:</strong>
      ${marks}
      </p>

      <p>
      <strong>Percentile:</strong>
      ${closest.percentile}
      </p>

      <p>
      <strong>Estimated Rank:</strong>
      ${closest.rank}
      </p>

      <h2 style="margin-top:20px;">
      Predicted Colleges
      </h2>

  `

  const filtered = colleges.filter(college =>

    college.category === category &&

    closest.rank <= college.closingRank * 1.1
  )

  if(filtered.length === 0){

    html += `
      <p style="margin-top:15px;">
      No colleges found.
      </p>
    `
  }

  filtered.forEach(college => {

    html += `

      <div class="college">

        <h3>${college.institute}</h3>

        <p>${college.branch}</p>

        <p>
        Closing Rank:
        ${college.closingRank}
        </p>

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
