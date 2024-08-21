send_html_or_pdf <- function (html, file_name, title, channel_id, send_html, initial_comment_value = "", ts = NULL) {
  if (isFALSE(send_html)) {
    system(glue(paste("xvfb-run wkhtmltopdf -O Landscape", "{html}", "{file_name}")))
  }
  response <- POST(
    url = "https://slack.com/api/files.upload",
    body = list(
      file = upload_file(glue("{file_name}")),
      title = glue("{title}"),
      filename = glue("{file_name}"),
      filetype = ifelse(send_html, "html", "pdf"),
      channels = channel_id,
      initial_comment = initial_comment_value,
      thread_ts = ts
    ),
    add_headers(.headers = c(
      Authorization = glue("Bearer {SLACK_BOT_TOKEN}"),
      `Content-Type` = "multipart/form-data"
    ))
  )
  
  return(content(response)$ts)
}