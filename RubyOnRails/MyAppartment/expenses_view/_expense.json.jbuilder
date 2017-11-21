json.extract! expense, :id, :common_expense, :particular_expense, :debt, :month, :created_at, :updated_at
json.url expense_url(expense, format: :json)